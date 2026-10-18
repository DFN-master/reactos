# Inventário de MCPs e Integrações

**Data:** 2026-10-18
**Fonte da verdade:** `opencode mcp list` (execução real) + documentação oficial do OpenCode.
**Nenhum pacote foi instalado. Nenhuma configuração global foi alterada nesta sessão.**

---

## 1. Estado atual — verificado

Comando executado:

```
opencode mcp list
```

Resultado (2 servidores, ambos **conectados**):

| Servidor | Estado | Comando | Origem da configuração |
|---|---|---|---|
| `ghidra` | **connected** | `C:\Users\Douglas\Downloads\liadmap\tools\ghidra-mcp\.venv\Scripts\python.exe -m bridge_mcp_ghidra --transport stdio` | **global** — `~/.config/opencode/opencode.json` |
| `rea` | **connected** | `cmd.exe /d /c npx -y rea-agents@6.5.0 mcp` | **global** — `~/.config/opencode/opencode.jsonc` |

Validação funcional não destrutiva do `rea`: a chamada `rea_binary_session` foi executada com sucesso e retornou a lista completa de providers e capacidades. **Comunicação comprovada.**

### Observação importante: há sobreposição entre `rea` e `ghidra`

O REA **já embute e gerencia** um backend Ghidra (12.1.3). Portanto existem **duas rotas de análise nativa** expostas simultaneamente ao agente:

- Via `rea_*` (207 ferramentas,Evidence formalizada, rastreamento de proveniência).
- Via `ghidra_*` (ferramentas mais diretas: breakpoints, watchpoints, stepping, `oracle_*`).

Isso **não é um erro**, e não vou desligar nada sem autorização. Mas é uma decisão de projeto que precisa ser explícita:

- O REA oferece **evidência estruturada e rastreável** (`ev_...` IDs, bundles, verificações), o que é valioso para um projeto que exige auditoria.
- O `ghidra_*` MCP expõe **depuração dinâmica** (`ghidra_debugger_*`, `ghidra_oracle_call_function`) que o REA **não** oferece neste host.

**Recomendação:** manter ambos, mas adotar o `rea` como rota **padrão** para análise estática com evidência, e reservar o `ghidra_*` para depuração dinâmica quando um caso justificar. Isso evita que dois agentes produzam conclusões paralelas sem rastreio.

---

## 2. Avaliação por MCP solicitado

### A. REA MCP — **OBRIGATÓRIO, JÁ INSTALADO E FUNCIONAL**

| Critério | Avaliação |
|---|---|
| Nome real do pacote | `rea-agents`, versão **6.5.0** (npx, `rea-agents@6.5.0`) |
| Origem | Servidor REA (author `morluto/rea`, conforme referência na skill global instalada) |
| Licença | **NÃO VERIFICADA** — ver limitação abaixo |
| Manutenção | Ativa — skill global reportando "expect v42" indica ciclo de releases em curso |
| Comando | `npx -y rea-agents@6.5.0 mcp` (stdio) |
| Permissões necessárias | Nenhuma especial. Sem escrita remota. Sem tokens observados. |
| Benefício concreto | Análise estática de binários nativos com **evidência estruturada**; é o backend necessário para comparar o comportamento real do Windows 7 contra o código do ReactOS. |
| Estado | **Instalado, conectado, validado.** Não requer ação. |

**Limitações reais neste host (declaradas pelo próprio servidor):**

- Provider Hopper: **indisponível** (`unsupported_host` — exige macOS).
- Provider IDA: **não configurado**.
- Ghidra no Windows: apenas **x86/x64 PE nativos** e raw 6502. **Não** suporta .NET/gerenciado.
- **Mutação de banco de dados desabilitada** (`unsupported_host`).

**⚠️ Pendência de compliance:** a **licença do pacote `rea-agents` não foi verificada** nesta auditoria. A regra 18 do projeto exige respeito às licenças de todas as ferramentas. Recomendo verificar a licença no repositório do pacote antes de qualquer uso contínuo e registrado como dependência do fluxo de trabalho. **Não fiz essa verificação porque exigiria acesso à rede ao repositório do pacote.**

---

### B. GitHub MCP — **RECOMENDADO, REQUER AUTORIZAÇÃO, NÃO INSTALADO**

| Critério | Avaliação |
|---|---|
| Nome real | **Não é um MCP.** É o **GitHub Agent nativo do OpenCode**: `opencode github install` / `opencode github run` |
| Origem | Embutido na distribuição do OpenCode (`opencode github --help` confirmou os subcomandos) |
| Licença | Mesma do OpenCode (a parte Agent) |
| Manutenção | Segue o ciclo do OpenCode |
| Benefício concreto | Consultar issues, PRs e revisões do **fork** e do **upstream ReactOS**; útil para localizar bugs já corrigidos upstream e evitar reinventar |
| Estado | **Não instalado.** Nenhuma conta foi conectada. |

**Riscos a considerar antes de autorizar:**
- Requer conectar uma conta GitHub — implica conceder acesso à sua conta.
- Permite escrita remota (criar branches, PRs, comentários) se as permissões não forem restritas.
- O `origin` deste projeto (`DFN-master/reactos`) pode ou não ser um repositório ao qual você wish write; **`upstream` (`reactos/reactos`) é de terceiros e NÃO deve receber escrita.**

**Configuração proposta:** instalar apenas com permissão de **leitura** initially. Especificamente:
- Sem permissão de escrita em `reactos/reactos` (upstream de terceiros).
- Sem permissão de criação de PR até que a fase de integração seja explicitamente autorizada.

**Alternativa sem instalação:** `git fetch upstream` + `git ls-remote` dão acesso de leitura ao upstream com zero novas dependências. **Para a finalidade atual (confirmar se o fork está sincronizado), isso é suficiente e mais simples.** Ver Seção 4.

---

### C. MCP de documentação — **DESCARTADO (por enquanto)**

| Critério | Avaliação |
|---|---|
| Necessidade | Baixa nesta fase |
| Ferramentas já disponíveis | `webfetch` e `websearch` nativos do OpenCode cobrem ReactOS Wiki, VirtualBox Manual, docs do CMake/Ninja/MSVC |
| Benefício de um MCP dedicado | Marginal — adicionaria um servidor, permissões e superfície de manutenção sem ganho mensurável agora |

**Decisão:** não instalar. A regra "não instale MCPs indiscriminadamente" aplica-se. Se surgir uma necessidade concreta (ex.: índice local da API do Windows/WDK), revisar.

---

### D. MCP de VirtualBox — **DESCARTADO, COM JUSTIFICATIVA**

**Nenhum MCP de VirtualBox foi instalado, e recomendo não instalar.**

Justificativa:
1. O `VBoxManage.exe` oficial (VirtualBox 7.2.6) expõe toda a operação necessária em modo de script: `list`, `showvminfo`, `snapshot`, `startvm`, `controlvm`, `storageattach`, `guestproperty`, e importantly `guestcontrol` (executa comandos no guest via guest additions).
2. Um MCP seria apenas um invólucro sobre essa CLI, adicionando uma dependência de terceiros e um modo de falha extra.
3. A skill `reactos-build-and-vm` e o agente `vm-lab-engineer` já operam diretamente sobre `VBoxManage`, o que é **auditável** (todo comando fica no histórico) — um MCP obscureceria a trilha de auditoria.

**Preferência explícita:** scripts locais controlados sobre `VBoxManage`, conforme instrução do projeto.

---

### E. Ferramentas de análise e build — **SEM MCP; usar CLI local**

Conforme instrução do projeto, Git, build, testes e análise de logs usam ferramentas locais:

| Necessidade | Ferramenta | Estado |
|---|---|---|
| Git | `git` 2.56.0 | OK |
| Build | `cmake` 4.3.2 + `ninja` 1.13.0 + MSVC 14.44.35207 | OK |
| VM | `VBoxManage.exe` 7.2.6 | OK |
| Análise estática | REA MCP (já MCP) | OK |
| Depuração dinâmica | `ghidra_*` MCP (já MCP) | OK |

---

## 3. Resumo de decisões

| MCP | Situação | Decisão |
|---|---|---|
| `rea` | Já instalado, conectado, validado | **Manter.** Rota padrão de análise estática com evidência. |
| `ghidra` | Já instalado, conectado | **Manter.** Reservar para depuração dinâmica. Sobreposição com `rea` é intencional e documentada. |
| GitHub | Não instalado | **PENDENTE DE AUTORIZAÇÃO.** Benefício real para consultar issues/PRs do upstream. Recomendo leitura-only. |
| Docs | Não instalado | **DESCARTADO.** `webfetch`/`websearch` nativos cobrem a necessidade. |
| VirtualBox | Não instalado | **DESCARTADO.** `VBoxManage` oficial é superior e auditável. |

---

## 4. Decisões que dependem de autorização do usuário

Abaixo estão as decisões que **não posso tomar sozinho**. Nenhuma foi executada.

| # | Decisão | Motivo da necessidade | Impacto |
|---|---|---|---|
| **D1** | Executar `git fetch upstream` | Altera refs locais; requer acesso de rede ao GitHub | Confirmaria se o fork está sincronizado com `reactos/reactos`. Sem isso, a relação fork↔upstream permanece **desconhecida** e não posso afirmar que o fork contém correções upstream recentes. **Baixo risco** (read-only no remoto). |
| **D2** | Liberar memória do host | Estado do sistema, não posso encerrar processos do usuário | **BLOQUEADOR CRÍTICO.** Com ~0,5 GB livres de 31,9 GB, o build do ReactOS e a VM não são viáveis. Precisa de ~8–16 GB livres. |
| **D3** | Criar VM ReactOS de teste | Cria arquivos e uma VM nova | Necessário para a baseline de boot. Disco novo, **jamais** reaproveitando o disco do Win7. |
| **D4** | Iniciar `Win7Test` (somente leitura) | Inicia uma VM; acesso ao Windows 7 de referência | Necessário para observar o comportamento real do Windows 7. Exige cuidado para não alterar a VM. |
| **D5** | Instalar GitHub Agent (`opencode github install`) | Instala software e conecta conta | Concede acesso à conta GitHub. Recomendo permissão **somente leitura**. |
| **D6** | Criar branch de trabalho (`nt61/<id>-<slug>`) | Altera refs Git | Necessário antes de qualquer commit. Regra do projeto: nunca commitar em `master`. |
| **D7** | Verificar licença do `rea-agents` | Requer acesso ao repositório do pacote | Conformidade com a regra 18 (licenças). |

---

## 5. Recomendações de prioridade

1. **D2 (memória)** — desbloqueia build e VM. **Prioridade máxima.**
2. **D1 (`fetch upstream`)** — desbloqueia o mapa de dependências e permite saber se há correções upstream aproveitáveis. Custo baixo.
3. **D6 (branch)** — habilita o fluxo Git sem poluir `master`.
4. **D3 + D4 (VMs)** — habilitam baseline de boot e comparação com o Win7.
5. **D7 (licença)** — conformidade.
6. **D5 (GitHub)** — conveniente, mas não bloqueia o trabalho atual.

---

## 6. O que NÃO foi feito (e não será sem autorização)

- Nenhum pacote foi instalado.
- Nenhum servidor MCP foi adicionado, removido ou reconfigurado.
- Nenhuma configuração **global** (`~/.config/opencode/*`) foi alterada.
- Nenhuma configuração local de projeto MCP foi criada.
- Nenhuma credencial foi criada, lida ou registrada. Este documento **não contém** nenhum token, senha ou chave.
- Nenhuma VM foi iniciada, criada, alterada ou destruída.
- Nenhuma operação Git de escrita foi executada.