# Inventário de Skills do Projeto

**Data:** 2026-10-18
**Host:** OpenCode 1.19.3
**Formato:** Agent Skills (`SKILL.md` com frontmatter YAML), conforme documentação oficial do OpenCode.

---

## 1. Onde as skills vivem e por que

Formato confirmado pela documentação oficial (`https://opencode.ai/docs/skills`), validado empiricamente com `opencode debug skill`.

Localização escolhida:

```
E:\Projeto ReactOS\reactos\.opencode\skills\<nome-da-skill>\SKILL.md
```

Locais suportados pela versão instalada, para referência:

| Tipo | Caminho |
|---|---|
| **Projeto** | `.opencode/skills/<name>/SKILL.md` ← **escolhido** |
| Projeto (formato Claude) | `.claude/skills/<name>/SKILL.md` |
| Projeto (formato agent) | `.agents/skills/<name>/SKILL.md` |
| Global | `~/.config/opencode/skills/<name>/SKILL.md` |

**Por que `.opencode/skills/` no projeto:**
1. É versionável no Git (a pasta global, não).
2. Fica junto da regra do projeto `reactos-git-integration`.
3. O OpenCode faz *discovery* subindo a árvore até a raiz do worktree Git, então funciona a partir de qualquer subdiretório do repositório.

---

## 2. Skills criadas

| # | Nome | Localização | Objetivo |
|---|---|---|---|
| 1 | `reactos-architecture` | `.opencode/skills/reactos-architecture/SKILL.md` | Mapear kernel/Executive, localizar a implementação de um comportamento NT, documentar caminho de execução e dependências. **Read-only.** |
| 2 | `rea-reverse-engineering` | `.opencode/skills/rea-reverse-engineering/SKILL.md` | Investigar comportamento real do Windows 7 via REA/Ghidra e produzir relatórios reproduzíveis. Proíbe tratar pseudocódigo como código-fonte e proíbe incorporar código proprietário. |
| 3 | `nt61-compatibility` | `.opencode/skills/nt61-compatibility/SKILL.md` | Manter inventário de compatibilidade NT 6.1 com máquina de estados **qualificada por arquitetura** (`TESTED_X86` ≠ `TESTED_X64`). |
| 4 | `reactos-build-and-vm` | `.opencode/skills/reactos-build-and-vm/SKILL.md` | Compilar com CMake+MSVC (`i386`/`amd64`) e validar boot em VM dedicada. Regras rígidas de proteção da VM de referência. |
| 5 | `reactos-regression-testing` | `.opencode/skills/reactos-regression-testing/SKILL.md` | Testes reproduzíveis, baseline obrigatória, classificação `REGRESSION` vs `PREEXISTING_FAILURE`, gate de aprovação. |
| 6 | `reactos-safe-implementation` | `.opencode/skills/reactos-safe-implementation/SKILL.md` | Mudanças mínimas e seguras; proibição de stubs que retornam sucesso; classificação de risco C1–C5. |
| 7 | `reactos-git-integration` | `.opencode/skills/reactos-git-integration/SKILL.md` | Branches isoladas, preservação de alterações preexistentes, proibições de commit/push sem autorização, dossiê pré-commit. |

---

## 3. Validação — como cada skill foi verificada

Não declarei nenhuma skill funcional apenas porque o arquivo existe. Foram duas verificações:

### 3.1 Validação estática de metadados

Para cada arquivo, verificado programaticamente:

- `SKILL.md` escrito em maiúsculas (requisito do loader).
- Frontmatter presente com `name` e `description`.
- `name` casa com o nome do diretório que o contém.
- `name` casa com `^[a-z0-9]+(-[a-z0-9]+)*$` (regra oficial).
- `1 <= len(description) <= 1024`.

**Resultado: 7/7 aprovados.**

### 3.2 Validação de descoberta pelo loader real

Comando executado na raiz do repositório:

```
opencode debug skill
```

Resultado: **9 skills descobertas**, sendo **7 do projeto**, todas com `location` apontando para
`E:\Projeto ReactOS\reactos\.opencode\skills\<nome>\SKILL.md`, e todas com o conteúdo carregado
na íntegra.

| Scope | Nome | Local |
|---|---|---|
| PROJECT | `reactos-regression-testing` | `./.opencode\skills\reactos-regression-testing\SKILL.md` |
| PROJECT | `reactos-architecture` | `./.opencode\skills\reactos-architecture\SKILL.md` |
| PROJECT | `reactos-safe-implementation` | `./.opencode\skills\reactos-safe-implementation\SKILL.md` |
| PROJECT | `nt61-compatibility` | `./.opencode\skills\nt61-compatibility\SKILL.md` |
| PROJECT | `reactos-git-integration` | `./.opencode\skills\reactos-git-integration\SKILL.md` |
| PROJECT | `reactos-build-and-vm` | `./.opencode\skills\reactos-build-and-vm\SKILL.md` |
| PROJECT | `rea-reverse-engineering` | `./.opencode\skills\rea-reverse-engineering\SKILL.md` |
| global | `reverse-engineer-anything` | `~/.agents/skills/...` (pré-existente, REA) |
| built-in | `customize-opencode` | embutida |

### 3.3 Limitação honesta da validação

**Ressalva registrada:** a **sessão OpenCode atual** foi iniciada **antes** da criação destas skills.
Por isso a ferramenta `skill` desta sessão só enxerga `customize-opencode` e `reverse-engineer-anything`
— o registro de skills é montado no **início da sessão**.

Isso significa:
- As skills **estão corretamente instaladas e descobertas** (confirmado por `opencode debug skill`, que
  é a ferramenta autoritativa do loader).
- Elas **ainda não são invocáveis pelo `skill(...)` nesta sessão específica.**
- **Serão carregadas automaticamente na próxima sessão iniciada dentro do repositório.**

Não afirmo que "a skill funcionou" porque a invokei — afirmo que o loader a descobriu, com evidência.

---

## 4. Como acionar cada skill

### 4.1 Carregamento automático (recomendado)

Todas as 7 foram criadas com:

```yaml
metadata:
  opencode/autoinvoke: "true"
```

Estão, portanto, na lista `<available_skills>` e o modelo pode carregá-las por relevance sem
intervenção humana.

### 4.2 Carregamento explícito

Pelo nome exato, quando a seleção automática não for adequada:

```
skill({ name: "reactos-architecture" })
```

Nomes válidos (case-sensitive, exatamente como no `name` do frontmatter):

```
reactos-architecture
rea-reverse-engineering
nt61-compatibility
reactos-build-and-vm
reactos-regression-testing
reactos-safe-implementation
reactos-git-integration
```

### 4.3 Uso por agente

Cada skill já declara qual agente a usa. Resumo:

| Skill | Agentes que a usam |
|---|---|
| `reactos-architecture` | `nt61-architect` (principal), `rea-researcher`, `reactos-implementer`, `reactos-reviewer` |
| `rea-reverse-engineering` | `rea-researcher` (principal), `nt61-architect` (leitura de evidência existente) |
| `nt61-compatibility` | `rea-researcher`, `integration-coordinator` (principal), `reactos-implementer`, `reactos-reviewer` |
| `reactos-build-and-vm` | `reactos-test-engineer`, `vm-lab-engineer` (principal) |
| `reactos-regression-testing` | `reactos-test-engineer` (principal), `reactos-implementer`, `reactos-reviewer`, `integration-coordinator` |
| `reactos-safe-implementation` | `reactos-implementer` (principal), `reactos-reviewer` |
| `reactos-git-integration` | `integration-coordinator` (principal) |

---

## 5. Eliminação de conflitos e duplicações

Revisado explicitamente. Resultado:

| Potencial conflito | Resolução |
|---|---|
| `reactos-architecture` vs `rea-reverse-engineering` — ambos "investigam" | Fronteira explícita: `reactos-architecture` lê **código-fonte aberto**; `rea-reverse-engineering` analisa **binários**. Nenhuma skill manda usar REA para ler fonte. |
| `reactos-safe-implementation` vs `reactos-regression-testing` — ambos exigem teste | Fronteira explícita: implementation **escreve** o teste; regression-testing **executa e classifica** o resultado. |
| `reactos-build-and-vm` vs `reactos-regression-testing` — ambos tratam de VM | Fronteira explícita: build-and-vm trata de **compilar e bootar**; regression-testing trata de **medir e classificar regressões**. |
| `reactos-git-integration` vs regra global "sem commit automático" | A skill é **mais restritiva** que a regra geral (bloqueia até `fetch`). Intencional: mais restritiva prevalece. |
| Declaração de "não é XP/2003" | Presente **apenas** em `reactos-architecture`, que é onde ela é relevante. Não duplicada. |
| Proibição de código proprietário | Presente em `rea-reverse-engineering` (fonte) **e** `reactos-safe-implementation` (destino). Duplicação **intencional e desejada**: a regra precisa existir nos dois lados da fronteira. |

Nenhuma instrução conflitante foi encontrada.

---

## 6. Permissões de skills

Não foi configurado bloco `permission.skill` — todas as 7 são permitidas por padrão.

Se no futuro alguma skill precisar ser restrita, o padrão suportado é:

```json
{ "permission": { "skill": { "*": "allow", "interno-*": "deny", "experimental-*": "ask" } } }
```

e por agente, via frontmatter do agente em `.opencode/agents/`.

---

## 7. Pendências

| Pendência | Motivo |
|---|---|
| Confirmação em sessão real | Requer **reiniciar a sessão OpenCode** no repositório. Até lá, o *discovery* está provado, mas a invocação ao vivo ainda não. |
| Verificação de conflito de nomes com skills globais | Nenhum conflito hoje. Se uma skill global passar a usar o mesmo nome, o loader **rejeita duplicatas** — monitorar. |
| Revisão periódica | As skills contêm fatos do ambiente (versões, caminhos) que envelhecem. Marcados como "verified 2026-10-18 — re-verify if stale". |

---

## 8. Addendum — upgrade do OpenCode para v2.0.32

Durante a execução desta fase, o CLI do OpenCode foi atualizado de **1.19.3** para **2.0.32** (provavelmente disparado por `npx -y rea-agents@6.5.0`). Consequências registradas:

- O subcomando `opencode debug skill` **não existe mais** em v2. A validação inicial de descoberta foi feita sob v1.19.3 (9 skills, 7 do projeto, com `location` correto) e permanece válida como evidência.
- A descoberta de skills em v2 é feita pelo loader a partir do diretório `.opencode/` (confirmado: `opencode debug config` lista `"type": "directory", "path": "E:\\Projeto ReactOS\\reactos\\.opencode"` como fonte de configuração).
- O formato `SKILL.md` com frontmatter é o mesmo nas duas versões (conforme a documentação oficial v2, que foi a fonte usada para criar as skills). Nenhuma alteração de formato foi necessária.
- **Os 7 agentes foram revalidados sob v2.0.32** via `opencode debug agents`: todos os 7 aparecem com os nomes corretos (`nt61-architect`, `rea-researcher`, `reactos-implementer`, `reactos-test-engineer`, `reactos-reviewer`, `vm-lab-engineer`, `integration-coordinator`), além dos agentes built-in.
- As permissões de cada agente foram verificadas como resolvidas corretamente (ex.: `reactos-reviewer` tem `edit: deny *`, `bash: deny *` + allows apenas para git read-only).

**Ação recomendada:** reiniciar a sessão OpenCode para carregar as skills e agentes recém-criados no contexto ativo.