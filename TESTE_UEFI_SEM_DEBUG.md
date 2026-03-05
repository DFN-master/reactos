# Guia para Testar ReactOS UEFI sem Debug Halt

## Opção 1: VirtualBox (RECOMENDADO)

### Configuração da VM:
1. **Criar Nova VM:**
   - Nome: ReactOS-UEFI-Test
   - Tipo: Other/Unknown
   - System → Motherboard: **✓ Enable EFI (special OSes only)**
   - System → Processor: 2+ CPUs
   - Display → Video Memory: 128 MB
   - Storage: IDE Controller → Adicionar bootcd.iso

2. **Configurar Porta Serial (CRÍTICO):**
   - Settings → Serial Ports → Port 1
   - ✓ Enable Serial Port
   - Port Mode: **"Disconnected"** ou **"Host Pipe"** com pipe inexistente
   - Isso fará o ReactOS ignorar a espera do debugger

3. **Iniciar a VM**

---

## Opção 2: QEMU com OVMF (Firmware UEFI)

```powershell
# Baixar OVMF (UEFI firmware para QEMU)
# Link: https://github.com/tianocore/edk2/releases

# Executar ReactOS com UEFI
qemu-system-x86_64.exe `
    -bios "C:\path\to\OVMF.fd" `
    -cdrom "e:\ReactOS\reactos\output-VS-amd64\bootcd.iso" `
    -m 4096 `
    -serial null `
    -vga std

# Parâmetro "-serial null" faz o debugger não travar
```

---

## Opção 3: VMware Workstation/Player

### Configuração:
1. Criar VM Windows Other 64-bit
2. **Ativar UEFI:**
   - VM Settings → Options → Advanced
   - Firmware type: **UEFI**
3. **Desabilitar Serial Port:**
   - VM Settings → Hardware
   - Remover ou desabilitar "Serial Port"
4. Adicionar bootcd.iso como CD-ROM
5. Iniciar

---

## Verificação UEFI

Quando o ReactOS iniciar, verifique no boot:
- ✅ Deve aparecer logo ReactOS sem erro "COM1"
- ✅ GPT: Sistema detectará partição UEFI
- ✅ Bootloader UEFI carregará automaticamente

---

## Se ainda aparecer "COM1 debugger":

Execute na VM (se conseguir acesso ao prompt):
```
bcdedit /debug off
bcdedit /bootdebug off
```

Ou edite boot\bootdata\BCD antes de recompilar.

---

## Arquivo da ISO:

**Localização:** `e:\ReactOS\reactos\output-VS-amd64\bootcd.iso`
**Tamanho:** 84 MB
**Data:** 05/03/2026 12:30:22
**Recursos:** UEFI + GPT + 22 DLLs modernas + Boot híbrido BIOS/UEFI
