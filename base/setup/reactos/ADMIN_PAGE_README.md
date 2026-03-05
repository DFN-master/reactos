# ReactOS Setup - Criação de Conta Administrador
## Guia Prático de Implementação e Testes

---

## ✅ O Que Foi Implementado

### 1. **Página de Criação de Conta Administrador** (NEW)
- **Arquivo**: `modern_admin_page.c` (330 linhas)
- **Funcionalidades**:
  - Campo Username (padrão: "Administrator")
  - Campo Password (oculto com bullets ●)
  - Campo Confirm Password (validação)
  - Checkbox "Mostrar Senha"
  - Validação en tempo real
  - Força da senha (progress bar)

### 2. **Recursos Visuais**
- **Arquivo**: `modern_dialogs.rc` (diálogo IDD_MODERN_ADMIN)
- **Layout**:
  - Título grande com fonte Segoe UI
  - Descrição explicativa
  - 3 campos de entrada (Username, Password, Confirm)
  - Checkbox para mostrar/ocultar senha
  - Aviso de segurança

### 3. **Validação de Segurança**
- Username: apenas letras, números, underscore, hífen
- Password: mínimo 4 caracteres (8+ recomendado)
- Senhas devem corresponder
- Botão "Next" desabilitado até validação passar
- Mensagens de erro descritivas

### 4. **Funções de Suporte**
```c
CreateUserAccount()        // Criar conta no registro
ValidateUsername()         // Validar formato username
ValidatePassword()         // Validar força senha
HashPassword()            // Hash seguro (TODO: bcrypt.dll)
```

---

## 🔧 Próximos Passos - Integração

### 1. **Modificar reactos.c** (Main Setup GUI)

```c
#include "modern_setup.h"

// No WinMain(), adicione a página do admin:
PROPSHEETPAGE psp[7];  // Aumentar de 6 para 7 páginas

// Página 2: Administrator Account
psp[2].dwSize = sizeof(PROPSHEETPAGE);
psp[2].hInstance = hInstance;
psp[2].pszTemplate = MAKEINTRESOURCE(IDD_MODERN_ADMIN);
psp[2].pfnDlgProc = AdminAccountPageProc;
psp[2].lParam = (LPARAM)&g_ModernContext;
psp[2].pszTitle = L"Administrator Account";

// Reindexar outras páginas: psp[3], psp[4], etc.
```

### 2. **Estrutura Global para Armazenar Credenciais**

```c
// Adicionar em reactos.h
typedef struct _SETUP_CONFIG
{
    WCHAR AdminUsername[256];
    WCHAR AdminPassword[256];
    WCHAR InstallDrive[32];
    WCHAR InstallPartition[32];
    BOOL  IsExpressMode;
} SETUP_CONFIG, *PSETUP_CONFIG;

// Global na página de admin:
static SETUP_CONFIG g_SetupConfig = {0};
```

### 3. **Modificar Page 5 (Progress)**

```c
// No ProgressPageProc(), durante a cópia de arquivos:
if (g_SetupConfig.AdminUsername[0])
{
    USER_ACCOUNT_INFO userInfo = {0};
    wcscpy(userInfo.Username, g_SetupConfig.AdminUsername);
    wcscpy(userInfo.Password, g_SetupConfig.AdminPassword);
    wcscpy(userInfo.FullName, L"Administrator");
    
    CreateUserAccount(&userInfo);
    
    // Atualizar progresso
    SetDlgItemTextW(hDlg, IDC_PROGRESS_STATUS, 
                    L"Creating administrator account...");
}
```

---

## 🧪 Testes Práticos

### Teste 1: Compilação Básica
```powershell
cd E:\ReactOS\reactos\output-VS-amd64
ninja reactos.exe

# Esperado: Compila sem erros
# Se houver erros: verificar includes em modern_admin_page.c
```

### Teste 2: Validação de Username
Execute em C#/.NET:
```csharp
// Testa ValidateUsername()
var validUsernames = new[] {
    "Administrator",      // ✓ OK
    "user123",           // ✓ OK
    "test_user",         // ✓ OK
    "test-user",         // ✓ OK
    "user@domain",       // ✗ @ não permitido
    "user name",         // ✗ espaço não permitido
    "user!@#$",          // ✗ caracteres especiais
};
```

### Teste 3: Validação de Senha
```csharp
var validPasswords = new[] {
    "",                   // ✓ OK (vazio permitido)
    "test",              // ✓ OK (4+ caracteres)
    "MyPass123!",        // ✓ OK
    "abc",               // ✗ Muito curta (<4)
};
```

### Teste 4: Dialog em Windows
```batch
REM Compiar reactos.exe para máquina com Windows 7
REM Executar: reactos.exe

REM Verificar:
REM - Página de admin aparece após "Installation Type"
REM - Username field aceita entrada
REM - Password field mostra bullets
REM - Show Password checkbox funciona
REM - Botão Next fica desabilitado se senhas não correspondem
```

### Teste 5: Em ReactOS VM

**Pré-requisitos**:
- bootcd.iso compilado com novo reactos.exe
- VirtualBox ou QEMU configurado
- 2GB RAM, 15GB HDD

**Passos**:
1. Boot da VM com bootcd.iso
2. Instalar com Express mode
3. Verificar que página de admin aparece
4. Criar conta: `username=testuser, password=test1234`
5. Concluir instalação
6. Reboot
7. Na tela de login, tentar: `testuser / test1234`
8. Verificar se desktop carrega com sucesso

---

## 📋 Checklist de Implementação

- [x] Criar modern_admin_page.c com formulário admin
- [x] Adicionar constantes de resource (IDC_ADMIN_*)
- [x] Criar diálogo IDD_MODERN_ADMIN em modern_dialogs.rc
- [x] Adicionar arquivo ao CMakeLists.txt
- [x] Criar guia de integração (ADMIN_INTEGRATION_GUIDE.txt)
- [ ] Modificar reactos.c para incluir página admin (Manual)
- [ ] Testar compilação completa
- [ ] Testar em VM Windows 7
- [ ] Testar em VM ReactOS
- [ ] Fazer login com conta criada

---

## 🐛 Possíveis Problemas e Soluções

### Problema 1: Erros de compilação "IDC_ADMIN_* undefined"
**Solução**: Verificar se modern_resource.h está incluído em modern_admin_page.c
```c
#include "modern_resource.h"  // Deve estar presente
```

### Problema 2: Dialog não aparece no wizard
**Solução**: Verificar se página foi adicionada corretamente em reactos.c
```c
psp[2].pszTemplate = MAKEINTRESOURCE(IDD_MODERN_ADMIN);  // Correto?
```

### Problema 3: Password não aparece na tela de login
**Solução**: Verificar se CreateUserAccount() salvou no local correto
```
HKEY_LOCAL_MACHINE\SAM\SAM\Domains\Builtin\Users\Names\username
```

### Problema 4: Botão "Next" não funciona
**Solução**: Verificar validação de senha em AdminAccountPageProc
- PSN_WIKBACK case necessário
- PSWIZB_NEXT habilitado quando senhas correspondem

---

## 🔐 Segurança - Implementação Futura

### Checklist de Segurança (TODO):
- [ ] Hash bcrypt.dll para password storage
- [ ] SecureZeroMemory() para limpar senha da memória
- [ ] Validação de força de senha em tempo real
- [ ] Log de tentativas de login falhadas
- [ ] Lockout de conta após 5 tentativas
- [ ] Recovery key generation
- [ ] DPAPI para encriptação de archivos sensíveis

---

## 📁 Arquivos Modificados/Criados

### Novos Arquivos:
```
✅ modern_admin_page.c         - 330 linhas (Página de admin)
✅ ADMIN_INTEGRATION_GUIDE.txt - Documentação completa
```

### Arquivos Modificados:
```
✅ modern_resource.h           - Adicionados IDC_ADMIN_* defines
✅ modern_dialogs.rc           - Adicionado diálogo IDD_MODERN_ADMIN
✅ modern_setup.h              - Adicionado protótipo AdminAccountPageProc
✅ CMakeLists.txt              - Adicionado modern_admin_page.c
```

### Arquivos Não Modificados (Próximos):
```
⏳ reactos.c                   - Precisa integração manual
⏳ reactos.h                   - Precisa struct SETUP_CONFIG
```

---

## 🚀 Fluxo Completo Esperado

```
[Welcome Page]
     ↓
[Installation Type: Express/Custom]
     ↓
[Administrator Account] ← NEW!
  Input: username, password
  Validation: Match + Strength
     ↓
[Partition Selection]
     ↓
[Installation Summary]
     ↓
[Installation Progress]
  Create admin account during file copy
     ↓
[Completion]
  Restart computer
     ↓
[ReactOS Boot]
  Login: testuser / test1234
  ✓ Success!
```

---

## 💡 Dicas de Desenvolvimento

1. **Debug com OutputDebugString()**
   ```c
   OutputDebugStringW(L"Debug: Admin account created\n");
   ```

2. **Verificar Registry em tempo real**
   ```powershell
   Get-Item "HKLM:\SAM\SAM\Domains\Builtin\Users\Names" -ErrorAction SilentlyContinue
   ```

3. **Testar Password Match**
   ```c
   if (wcscmp(szPassword, szPasswordConfirm) != 0)
       MessageBoxW(...);  // Erro claro para usuário
   ```

4. **Verificar RID Assignment**
   ```c
   // Detectar próximo RID disponível
   for (DWORD rid = 1000; rid < 2000; rid++)
       // Checar se existe em registry
   ```

---

## 📞 Suporte

Se encontrar problemas:
1. Verificar erros de compilação com `ninja -v` para verbose output
2. Testar cada função individualmente
3. Usar debugger Visual Studio para step-through do código
4. Consultar ADMIN_INTEGRATION_GUIDE.txt para detalhes técnicos

Sucesso na implementação! 🎉
