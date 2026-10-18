# Auditoria Inicial do Ambiente — ReactOS NT 6.1

**Data:** 2026-10-18
**Executor:** Agente coordenador (OpenCode)
**Escopo:** Auditoria somente-leitura do repositório, ferramentas de desenvolvimento e máquinas virtuais. Nenhuma alteração de código, VM, disco, branch ou configuração global foi realizada.

---

## Legenda de classificação

Cada afirmação neste documento está classificada como:

| Marca | Significado |
|---|---|
| `[FATO]` | Verificado diretamente por comando nesta auditoria |
| `[HIPOTESE]` | Inferência razoável, ainda não comprovada |
| `[INCONCLUSIVO]` | Verificação iniciada mas não concluída |
| `[NAO INVESTIGADO]` | Fora do alcance desta auditoria |

---

## 1. Repositório

### 1.1 Identidade e localização `[FATO]`

| Item | Valor |
|---|---|
| Caminho absoluto | `E:\Projeto ReactOS\reactos` |
| É um fork do ReactOS | Sim — `upstream` aponta para `https://github.com/reactos/reactos.git` |
| Branch atual | `master` |
| Commit (`HEAD`) | `0345c201b4f02c63d5c69b7c871fc285e0a1e285` |
| Commit (mensagem) | `[HALX86] Add header file for x2APIC (#9441)` |
| Data do commit | `2026-09-16 12:14:32 +0200` |
| `git describe` | `0.4.16-dev-3517-g0345c201b4f` |
| Versão declarada em `sdk/include/reactos/version.cmake` | `0.4.17-dev`, `COPYRIGHT_YEAR 2026` |
| Total de commits no histórico | 89 541 |
| Working tree | **LIMPO** — `git status --porcelain` retornou 0 linhas |
| Submódulos | **Nenhum** — `git submodule status` retornou vazio |
| Licença | GNU GPL 2.0 (`COPYING`) |

> Observação factual: o `README.md` referencia a release 0.4.16 e o badge do botão de download aponta para 0.4.16, enquanto `version.cmake` já declara `0.4.17`. Isso é consistente com um repositório de desenvolvimento pós-0.4.16.

### 1.2 Remotes `[FATO]`

```
origin    https://github.com/DFN-master/reactos.git   (fetch e push)
upstream  https://github.com/reactos/reactos.git      (fetch e push)
```

- `HEAD` é idêntico a `origin/master` (0 commits à frente, 0 atrás). `[FATO]`
- `refs/remotes/upstream/master` **não existe localmente** — o remote `upstream` está configurado mas **não foi buscado** (`git rev-parse --verify refs/remotes/upstream/master` → `fatal: Needed a single revision`). `[FATO]`
- Consequência: **não é possível afirmar, nesta auditoria, que o fork está sincronizado com o upstream.** Comparar exigiria um `git fetch upstream` (operação de rede, somente leitura do remoto, mas altera refs locais).

**Limitação registrada:** a relação entre o fork e o upstream permanece **INCONCLUSIVA** até que um `fetch` seja autorizado e executado.

### 1.3 Alterações locais preexistentes

**Nenhuma.** O working tree está limpo, portanto não há alterações do usuário a preservar neste momento. `[FATO]`

Ainda assim, a partir de agora, qualquer arquivo criado neste projeto (diretórios `research/`, `nt61/`, `tests/`, `agents/`, `.opencode/`) é **adicionado não rastreado** e deve ser tratado como pertencente a este trabalho.

### 1.4 Estrutura de diretórios `[FATO]`

Componentes de alto nível presentes na raiz:

| Diretório | Responsabilidade |
|---|---|
| `ntoskrnl/` | Kernel do ReactOS (executivo, kernel) |
| `win32ss/` | Subsistemas de usuário (win32k, user32, gdi32 etc.) |
| `sdk/` | Cabeçalhos e definições compartilhadas (`include/reactos/version.cmake`) |
| `base/` | Bibliotecas base em user mode |
| `dll/` | DLLs win32 (ntdll, kernel32, advapi32, etc.) |
| `drivers/` | Drivers do sistema |
| `hal/` | Camada de abstração de hardware |
| `subsystems/` | Subsistemas de sistema (mconfig, test e afins) |
| `boot/` | Bootloaders e ambiente de build |
| `modules/` | Módulos diversos (ex.: drivers de rede) |
| `media/` | Drivers de mídia |
| `.github/`, `.theia/` | CI e configuração de IDE |

Não há diretórios pré-existentes `research/`, `nt61/`, `tests/` ou `agents/` — todos foram **criados por esta auditoria** e não colidem com estrutura existente do ReactOS.

### 1.5 Testes disponíveis `[FATO / PARCIAL]`

- O ReactOS possui suíte de testes externa: **Testman** (`https://reactos.org/testman/`), referenciada no `README.md`.
- O repositório **não contém** a suíte Testman localmente (não há `tests/` nem pasta equivalente no working tree). `[FATO]`
- A cobertura de testes executável dentro deste repositório é **limitada a builds e a testes unitários pontuais**; não foi feita uma varredura exaustiva dos alvos de teste do CMake nesta auditoria. `[INCONCLUSIVO]`

### 1.6 Instruções de build aplicáveis à revisão `[FATO]`

Fontes no repositório: `README.md`, `INSTALL`, `CMakePresets.json`, `CMakeLists.txt`.

O `CMakeLists.txt` (linhas 31–49) exige a variável `ARCH` e aceita explicitamente:

```
i386 | amd64 | arm | arm64
```

Para `ARCH=i386`, `WINARCH` é definido como `x86`.

Fluxo de build oficial segundo `INSTALL` e `README.md`:

1. Executar `configure.cmd` (Windows) no diretório de build desejado — **ou** usar `cmake --preset <preset>`.
2. `ninja` para compilar tudo; `ninja <módulo>` para um módulo específico.
3. `ninja bootcd` para gerar a imagem de CD inicializável.
4. `ninja install` com `ROS_INSTALL` definido para instalar sobre uma instalação existente.

**Arquiteturas suportadas por esta revisão:** `i386` (x86), `amd64` (x64), `arm`, `arm64`.

**Presets CMake disponíveis** (`CMakePresets.json`, versão 3, exige CMake ≥ 3.19):
- `mingw-{i386,amd64}-{debug,release}` → `output-MinGW-i386`, `output-MinGW-amd64`
- `msvc-{i386,amd64,arm,arm64}-{debug,release}` → `output-VS-*`
- `clang-cl-i386-*` → `output-VS-i386`
- `clang-{i386,amd64,arm,arm64}-{debug,release}` → `output-Clang-*`
- `vs2022-sln-{i386,amd64,arm,arm64}` → soluções Visual Studio

> **Relevância para NT 6.1:** x86 e x64 são as arquiteturas de interesse para Windows 7. `i386` e `amd64` estão cobertas por presets em todas as três famílias de toolchain.

---

## 2. Ferramentas de desenvolvimento

### 2.1 Inventário verificado `[FATO]`

| Ferramenta | Versão / Estado | Caminho | Situação |
|---|---|---|---|
| Git | `2.56.0.windows.2` | `C:\Program Files\Git\cmd\git.exe` | OK |
| OpenCode | `1.19.3` | `C:\Users\Douglas\AppData\Roaming\npm\opencode.ps1` | OK |
| CMake | `4.3.2` | `C:\Program Files\CMake\bin\cmake.exe` | OK |
| Ninja | `1.13.0.git.kitware.jobserver-pipe-1` | `C:\Users\Douglas\AppData\Roaming\Python\Python314\Scripts\ninja.exe` | OK, **fora do PATH** |
| Python | `3.14.4` | `C:\Python314\python.exe` | OK |
| MSVC 2022 Community | Toolset `14.44.35207` | `C:\Program Files\Microsoft Visual Studio\2022\Community` | OK (`cl.exe` x86 e x64 presentes) |
| MSVC 2019 Build Tools | presente | `C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools` | OK (não verificado em detalhe) |
| Build Tools "18" (2026) | Toolset `14.50.35717` | `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools` | OK (não verificado em detalhe) |
| Windows SDK | `14393, 15063, 16299, 17134, 18362, 19041, 26100` | `C:\Program Files (x86)\Windows Kits\10\bin` | OK |
| `vcvarsall.bat` | presente | `...\2022\Community\VC\Auxiliary\Build\vcvarsall.bat` | OK |
| Clang / LLVM-MinGW | `22.1.2` (UCRT, x86_64) | `...\llvm-mingw-20260324-ucrt-x86_64\bin` | OK (veja ressalva) |
| `gcc` | resolving to Clang 22.1.2 | mesmo diretório | **Atenção:** `gcc` no PATH é um shim do clang |
| VirtualBox CLI | `7.2.6r172322` | `C:\Program Files\Oracle\VirtualBox\VBoxManage.exe` | OK, **fora do PATH** |
| RosBE | **não encontrado** | — | Ausente |
| `rea` (executável CLI) | **não encontrado no PATH** | — | Ausente como CLI |

### 2.2 REA e backend de análise `[FATO]`

O REA **não está instalado como executável de linha de comando**, mas o **servidor MCP do REA está configurado e conectado** nesta sessão.

Origem da configuração (arquivo global, **não** do projeto):
`C:\Users\Douglas\.config\opencode\opencode.jsonc`, bloco `"mcp"` → chave `"rea"`, `"type": "local"`, `"enabled": true`.

Validação não destrutiva executada: chamada `rea_binary_session` retornou com sucesso, provando comunicação ponta a ponta.

Identidade do servidor e capacidades (extraídos da resposta):

| Item | Valor |
|---|---|
| Pacote | `rea-agents` **v6.5.0** |
| Servidor MCP | `rea` 6.5.0 |
| Cliente | `opencode` 1.19.3 |
| Protocolo MCP | `2025-11-25` (SDK 2.3.1) |
| Backend disponível | **Ghidra 12.1.3** (único provider utilizável) |
| Ghidra (instalação) | `C:\Users\Douglas\Downloads\liadmap\tools\ghidra\ghidra_12.1.3_PUBLIC` |
| Java | Eclipse Adoptium JDK `21.0.12.1` (64-bit) |
| Alvo aberto | nenhum (`open = false`) |
| Catálogo | 153 comandos CLI, 207 ferramentas MCP, 6 prompts |

**Limitações do REA neste host (fatos declarados pelo próprio servidor):**
- Provider `hopper`: **indisponível** (`unsupported_host` — requer macOS).
- Provider `ida`: **não configurado** (requer `REA_IDA_MCP_CONFIG`).
- Ghidra no Windows: suporta **apenas binários nativos x86/x64 PE e DLLs** (e raw 6502).
- **Mutação de banco de dados desabilitada** (`unsupported_host`); edições de metadados são apenas de sessão.
- Skill `reverse-engineer-anything` instalada: versão reportada pelo servidor é anterior à esperada (skill reportada como `expect v42`).

### 2.3 Ferramentas classificadas `[FATO]`

**Obrigatórias para esta fase:**
- Git, CMake, Ninja, MSVC (x86 + x64), Windows SDK, VirtualBox CLI (`VBoxManage`).

**Obrigatórias apenas para a análise via REA:**
- Ghidra 12.1.3 + JDK 21 (já presentes).

**Opcionais / possivelmente dispensáveis:**
- Clang/LLVM-MinGW 22.1.2 — presente, mas `toolchain-clang.cmake` não é o caminho recomendado pelo `README.md` (que indica RosBE ou MSVC). Versão 22.1.2 é muito recente; **não verificada** contra os requisitos desta revisão. `[INCONCLUSIVO]`
- RosBE — ausente. Não é obrigatório, pois `README.md` documenta alternativa com MSVC 2019+.
- Build Tools 2019 e 2026 — provavelmente redundantes para a revisão atual.

**Indisponíveis (impacto registrado):**
- Não há ferramenta de teste de automação de VM pronta; a automação de VirtualBox precisará de scripts próprios sobre `VBoxManage`.

---

## 3. Recursos do host — CONSTRAINTS CRÍTICOS

`[FATO]` Medições em 2026-10-18:

| Recurso | Valor |
|---|---|
| CPU física | Intel Core i7-11700 @ 2.50GHz, 16 threads lógicos / 8 cores |
| Virtualização de hardware | suportada (VT-x/AMD-V), PAE, long mode, nested paging; **nested HW virtualization: não** |
| Memória total | **31,9 GB** |
| **Memória livre no momento da auditoria** | **≈ 0,5 GB** |
| Disco `C:` | 44,7 GB livres |
| Disco `D:` | 134,9 GB livres |
| Disco `E:` | 595,5 GB livres |
| SO do host | Windows 11, build `10.0.26300.9711` |

> **RISCO CRÍTICO REGISTRADO:** com apenas ~0,5 GB de RAM física livre, um build completo do ReactOS em paralelo com VMs não é viável na configuração atual. Isso precisa ser resolvido (liberar RAM / encerrar processos pesados) **antes** da fase de baseline. Não é uma limitação do ReactOS; é do estado do host.

`[HIPOTESE]` A RAM baixa é causada por outros processos/VMs residentemente em execução no host. **Não foi investigated** — nenhum processo foi inspecionado, e nenhum foi encerrado.

---

## 4. Máquinas virtuais

### 4.1 VirtualBox `[FATO]`

| Item | Valor |
|---|---|
| Versão | `7.2.6r172322` |
| Caminho | `C:\Program Files\Oracle\VirtualBox\VBoxManage.exe` (fora do PATH) |
| VMs registradas | 4 |
| VMs em execução | **0** |

### 4.2 Inventário de VMs `[FATO]`

| Nome | UUID | SO declarado | RAM | CPU | Rede | Firmware |
|---|---|---|---|---|---|---|
| `Ubuntu Server - Lara` | `f0bcb3ba-242b-4bed-88a4-41160bc8fd81` | Ubuntu (64-bit) | 2933 MB | 4 | bridged | BIOS |
| `Win7USB` | `1e11eee0-cdd5-4edc-baf3-c4fe3b9e9cc6` | **Windows 7 (32-bit)** | 2048 MB | 1 | nat | BIOS |
| `Win7Test` | `dfe545db-9d7e-4db4-b981-612e9cf20243` | **Windows 7 (32-bit)** | 2048 MB | 1 | none | BIOS |
| `Windows 10` | `91ff432c-7cfa-4c35-8e11-208a6b0c6cb7` | Windows 10 (64-bit) | 4096 MB | 6 | bridged | BIOS |

**Snapshots: nenhuma VM possui snapshots.** (Verificado em todas as 4.)

### 4.3 Armazenamento das VMs Windows 7 `[FATO]`

**`Win7Test`** — candidata a VM de referência do Windows 7:

| Controlador | Porta | Arquivo | Tamanho | Estado |
|---|---|---|---|---|
| IDE (PIIX4, bootável) | 0,0 | `D:\Win7Test.vdi` | 5.329.911.808 bytes | **EXISTE** |
| IDE (PIIX4, bootável) | 1,0 | `D:\Win7ProSP1_ptBR_x86.iso` | 2.434.740.224 bytes | **EXISTE** |
| SATA (IntelAhci, não bootável) | 0,0 | `C:\Users\Douglas\AppData\Local\Temp\opencode\win7usb.vmdk` | — | **ARQUIVO AUSENTE** |
| SATA (IntelAhci, não bootável) | 1,0 | `C:\Users\Douglas\AppData\Local\Temp\opencode\cmp.iso` | — | **ARQUIVO AUSENTE** |

**`Win7USB`**:

| Controlador | Porta | Arquivo | Estado |
|---|---|---|---|
| SATA (IntelAhci, bootável) | 0,0 | `C:\Users\Douglas\AppData\Local\Temp\opencode\win7usb.vmdk` | **ARQUIVO AUSENTE** |

### 4.4 Achados de segurança e integridade `[FATO]`

1. **`Win7USB` está quebrada.** Seu único disco (`win7usb.vmdk`, UUID `19176b2b-6fcd-46cb-bfb3-8b48480216bc`) **não existe no disco**. A VM não pode iniciar.
2. **Disco compartilhado entre duas VMs.** `Win7Test` (SATA 0,0) e `Win7USB` (SATA 0,0) apontam para o **mesmo arquivo e o mesmo UUID de medium** (`win7usb.vmdk` / `19176b2b-...`). O VirtualBox proíbe iniciar duas VMs com o mesmo medium. Mesmo que o arquivo existisse, iniciar ambas simultaneamente seria impossível — e o uso de um mesmo VMDK por duas VMs é um risco conhecido de corrupção.
3. **`Win7Test` tem anexos pendentes.** Dois anexos SATA apontam para arquivos inexistentes. A VM pode iniciar (o IDE é o bootável e o disco real existe), mas os dispositivos SATA vão gerar erros e perder o `cmp.iso`.
4. **Nenhum snapshot existe** em qualquer VM — portanto não há "estado bom conhecido" para restaurar.
5. **Não há VM ReactOS.** Não existe máquina virtual para testar builds do ReactOS.
6. **Não existe VM Windows 7 x64.** Ambas as VMs Win7 são **32-bit (x86)**. Isso limita a comparação com o Windows 7 x64.

### 4.5 VM de referência — decisão pendente `[INCONCLUSIVO]`

**Fatos:** existem **duas** VMs declaradas como "Windows 7 (32-bit)". Apenas `Win7Test` possui disco real e existente.

**Hipótese (a confirmar):** `Win7Test` é a VM de referência do Windows 7, porque:
- possui disco persistente em `D:\Win7Test.vdi` de 5,3 GB (tamanho plausível para uma instalação do Win7);
- mantém a ISO de instalação `D:\Win7ProSP1_ptBR_x86.iso` (edição Pro SP1, x86, pt-BR) anexada;
- tem a rede desligada (`nic1=none`), o que é coerente com uma VM de teste isolada.

**Não foi verificado:** se o sistema dentro de `D:\Win7Test.vdi` realmente inicializa, qual o Service Pack exato, quais versões de API estão presentes, ou se é a instalação desejada.

> **A VM de referência NÃO foi iniciada, modificada, clonada, redimensionada, nem teve disco ou snapshot alterado durante esta auditoria.** Nenhuma VM foi iniciada em nenhum momento.

### 4.6 Viabilidade de uma VM ReactOS separada `[FATO + HIPOTESE]`

- Há espaço em disco suficiente em `D:` (134,9 GB) e `E:` (595,5 GB). `[FATO]`
- Não há RAM livre suficiente no momento (~0,5 GB de 31,9 GB). `[FATO]`
- `[HIPOTESE]` Uma VM ReactOS x86 de 1–2 GB é viável **após** liberar memória no host, e pode ser criada com disco **novo** em `D:\` ou `E:\`, nunca reaproveitando `D:\Win7Test.vdi`.
- **Nenhuma VM foi criada nesta auditoria.**

---

## 5. Verificações pendentes e itens não investigados

### 5.1 Pendências `[INCONCLUSIVO]`

| # | Item | Por que está pendente |
|---|---|---|
| 1 | Relação fork ↔ upstream do ReactOS | Requer `git fetch upstream` (acesso de rede a refs) |
| 2 | Estado real do Windows 7 dentro de `D:\Win7Test.vdi` | Exige iniciar a VM — requer autorização explícita |
| 3 | Service Pack / build exatos do Windows 7 de referência | Idem |
| 4 | Cobertura de testes do CMake desta revisão | Varredura não realizada |
| 5 | Compatibilidade de Clang 22.1.2 com `toolchain-clang.cmake` | Requer tentativa real de configure |
| 6 | Configuração do build MSVC desta revisão | Requer executar `vcvarsall` + `cmake --preset` |
| 7 | Causa da baixa memória livre no host | Nenhum processo foi inspecionado |

### 5.2 Não investigado `[NAO INVESTIGADO]`

- Existência/qualidade de testes unitários dentro do repositório.
- Estrutura interna do kernel para os itens de arquitetura posteriores (será objeto da skill `reactos-architecture`).
- Viabilidade real de `ninja bootcd` (build completo — não iniciado).
- Fluxo de `ninja install` e requisitos de boot.
- Integração GitHub (issues/PRs do projeto).

---

## 6. Restrições e regras derivadas desta auditoria

1. **Nenhuma VM foi iniciada, parada, clonada ou alterada.** `Win7Test` e `Win7USB` permanecem exatamente como estavam.
2. **Nenhuma configuração global foi alterada.** O `opencode.json` / `opencode.jsonc` globais não foram tocados.
3. **Nenhum commit, push, merge ou alteração de branch foi realizado.**
4. **Working tree estava limpo no início** e apenas diretórios/arquivos novos, não rastreados, foram adicionados.
5. **`Win7Test` é tratada como VM de referência e é read-only por política.** Antes de qualquer uso, deve-se considerar criar um clone/snapshot apenas com autorização.
6. **`Win7USB` está quebrada** (disco ausente). Não deve ser reparada nem seus anexos removidos sem autorização, pois a remoção de anexo altera a configuração da VM.
7. **RAM do host é o bloqueio principal** para build e para VM simultânea.
8. **REA/Ghidra no Windows só analisa binários nativos x86/x64 PE.** Isso **é suficiente** para investigar `ntdll.dll`, `kernel32.dll`, etc. do Windows 7 de referência, mas **não** permite análise de .NET gerenciado nem de imagens não-PE.

---

## 7. Conclusão da auditoria

O ambiente é **utilizável**, com as seguintes condições:

- ✅ Repositório ReactOS fork íntegro, limpo e com instruções de build claras (CMake + Ninja, `i386`/`amd64`).
- ✅ Toolchain de build essencial disponível: MSVC 2022 (x86 + x64), CMake, Ninja (apenas precisando ir ao PATH).
- ✅ REA MCP **conectado e funcional** com Ghidra 12.1.3 — adequado para análise de binários nativos do Win7 x86.
- ✅ VirtualBox 7.2.6 disponível com uma VM Windows 7 x86 com disco real (`Win7Test`).
- ⚠️ **Bloqueio 1:** apenas ~0,5 GB de RAM livre no host — precisa ser resolvido antes do baseline.
- ⚠️ **Bloqueio 2:** `Win7USB` quebrada e `Win7Test` com anexos pendentes; nenhuma das duas deve ser tocada sem autorização.
- ⚠️ **Pendência 3:** relação com o upstream do ReactOS não confirmada.
- ⚠️ **Limitação 4:** não há VM Windows 7 **x64**, e o REA/Ghidra no Windows também é limitado a x86/x64 PE.

**Nenhuma funcionalidade de compatibilidade NT 6.1 deve ser implementada antes de resolver o bloqueio de memória e estabelecer a baseline de build.**

---

## 8. Addendum — tentativa de baseline (2026-10-18)

Após esta auditoria, foi tentada a configuração de build (CMake `--preset msvc-i386-debug` e `msvc-amd64-debug`) com MSVC 2022 + CMake 4.3.2 + Ninja 1.13.0. **Ambas falharam de forma idêntica** em `CMakeLists.txt:97` com `Could NOT find BISON (missing: BISON_EXECUTABLE)`.

Consequência: Bison (e provavelmente Flex e M4) **não estão instalados** no host. A falha é de ferramenta, não de código.

Relatório completo em: `research/environment/build-baseline.md`.

**Bloqueador resolvido:** Bison/Flex/M4 foram instalados via WinFlexBison 2.5.25 (winget), com autorização do usuário. Build completo bem-sucedido para i386 e amd64; `bootcd.iso` gerado para ambas as arquiteturas. Ver `research/environment/build-baseline.md` para o resultado final.