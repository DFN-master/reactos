# Proposta do Primeiro Experimento de Compatibilidade NT 6.1

**Data:** 2026-10-18
**Autor:** Agente coordenador (OpenCode)
**Estado:** **PROPOSTA, NÃO INICIADA.** Nenhuma API foi selecionada. Nenhuma implementação foi feita.

---

## 1. Honestidade sobre o estado

A instrução do projeto é clara: *"Não escolha antecipadamente uma API apenas para ter algo para modificar. Escolha a primeira tarefa com base nas evidências da baseline."*

A baseline **não está estabelecida**. A configuração de build falhou por falta de Bison/Flex/M4 no host (ver `research/environment/build-baseline.md`). Sem build, não há testes executáveis, sem testes não há evidência de falha, sem evidência de falha não há critério para escolher a primeira API.

**Portanto, este documento não seleciona uma API.** Ele define:

1. O **critério de seleção** (já existe na skill `nt61-compatibility`).
2. O **processo** que levará à seleção.
3. Um **curto-circuito de segurança** para que ninguém tente "adivinhar" uma API no meio do caminho.

Se alguém (humano ou agente) propor uma API específica antes que a baseline exista, essa proposta deve ser **rejeitada** até que as evidências da Seção 3 estejam disponíveis.

---

## 2. Critérios de seleção (obrigatórios — todos devem ser atendidos)

Em ordem de prioridade, conforme `nt61-compatibility`:

1. **Alto impacto para aplicações reais** — a lacuna quebra visivelmente aplicações Win32 comuns.
2. **Superfície pequena e delimitada** — uma função ou um pequeno subsistema, comprovável com um único teste.
3. **Baixo acoplamento** — poucos chamadores/callees; risco de regressão baixo.
4. **Observável** — existe um sinal claro de passa/falha reproduzível.

Um candidato é **rejeitado** se for:
- De escopo kernel-wide (boot, MM, scheduler) nesta fase.
- Vago ("melhorar compatibilidade Win32" não é um item).
- Escolhido apenas porque o nome da função "parece faltar".

---

## 3. Processo de seleção (a seguir, nesta ordem)

```
┌─────────────────────────────────────────────────────────────┐
│ 1. Instalar Bison/Flex/M4 (autorização necessária)          │
│    → Ver build-baseline.md, Seção 6                          │
├─────────────────────────────────────────────────────────────┤
│ 2. Reexecutar configure (i386 + amd64)                       │
│    → Confirmar que passa de find_package(BISON)             │
├─────────────────────────────────────────────────────────────┤
│ 3. Build completo (ninja) por arquitetura                   │
│    → Registrar artefatos e duração                          │
├─────────────────────────────────────────────────────────────┤
│ 4. Criar VM ReactOS (autorização + disco novo)              │
│    → Snapshot baseline imediatamente                         │
├─────────────────────────────────────────────────────────────┤
│ 5. Boot + coleta de debug log                               │
│    → Confirmar que o sistema sobe                            │
├─────────────────────────────────────────────────────────────┤
│ 6. Rodar suíte de testes existente                           │
│    → Registrar pass/fail por teste, por arquitetura          │
├─────────────────────────────────────────────────────────────┤
│ 7. Identificar o PRIMEIRO teste que falha                    │
│    → Esse é o candidato inicial (evidência, não palpite)    │
├─────────────────────────────────────────────────────────────┤
│ 8. Investigar com rea-researcher + nt61-architect            │
│    → Confirmar que a falha é um gap de compatibilidade      │
│      real, não um bug de teste ou de ambiente                │
├─────────────────────────────────────────────────────────────┤
│ 9. Escrever acceptance criteria + teste que falha hoje       │
├─────────────────────────────────────────────────────────────┤
│ 10. Implementar (reactos-implementer) + revisar             │
│     (reactos-reviewer) + gate (reactos-test-engineer)        │
└─────────────────────────────────────────────────────────────┘
```

**A seleção real acontece no passo 7, com evidência.** Nenhuma API será nomeada antes disso.

---

## 4. Categorias de candidatos esperadas (hipóteses, não decisões)

Com base na estrutura do repositório e na natureza da revisão (0.4.17-dev, era NT 5.2), as categorias **mais prováveis** de conter o primeiro experimento adequado são:

| Categoria | Por que é candidata | Por que pode não ser |
|---|---|---|
| **Wrapper de user-mode em `kernel32`/`ntdll`** que chama uma primitiva já existente no kernel | Superfície pequena, baixo acoplamento, testável em user mode | Pode já estar completo |
| **Uma syscall `Nt*`/`Zw*`** cuja implementação no ReactOS é parcial ou retorna `STATUS_NOT_IMPLEMENTED` | Fronteira clara, testável via ntdll | Pode exigir mudanças no kernel (C3/C4) |
| **Função de Registro/Configuração** (ex.: `Reg*`, `Rtl*`) com divergência de código de erro | Alto impacto para apps, observável | Pode ser amplo demais |
| **Função de string/heap em `ntdll`** (`Rtl*`) com comportamento divergente | Isolado, testável | Impacto pode ser baixo |

**Nenhuma dessas categorias é uma decisão.** São hipóteses de onde o primeiro teste que falhar provavelmente estará. A escolha final será feita pelos dados do passo 7 do processo acima.

---

## 5. O que **não** fazer (anti-padrões que bloqueariam a aprovação)

- ❌ Escolher uma API porque "parece faltar" sem evidência de falha.
- ❌ Escolher uma API de boot/MM/scheduler nesta fase.
- ❌ Escolher uma API "para ter algo para modificar".
- ❌ Implementar antes de escrever o teste.
- ❌ Implementar antes de obter a evidência do Windows 7.
- ❌ Tratar resultado x86 como prova para x64.

---

## 6. Pré-requisitos que ainda faltam (bloqueadores)

| Bloqueador | Status | Ação necessária |
|---|---|---|
| Bison/Flex/M4 ausentes | **RESOLVIDO** | WinFlexBison 2.5.25 instalado via winget (autorizado). Build completo OK para i386 e amd64. |
| RAM livre | **GERENCIADO** | Build concluído com ~3–6 GB livres usando `ninja` (paralelismo padrão). |
| VM ReactOS de teste | **NÃO EXISTE** | Autorização para criar (disco novo) |
| VM Windows 7 de referência observada | **NÃO INICIADA** | Autorização para boot read-only |
| Primeiro teste que falha | **DESCONHECIDO** | Depende dos passos anteriores (sem VM, sem suite local) |

---

## 7. Próximas ações (ordenadas)

1. **Obter autorização** para instalar RosBE (ou Bison/Flex/M4 standalone) — desbloqueia todo o resto.
2. **Obter autorização** para encerrar processos pesados / liberar RAM.
3. Reexecutar configure + build (i386 e amd64).
4. **Obter autorização** para criar VM ReactOS e bootar build (read-only na referência).
5. Rodar a suíte de testes existente e registrar a baseline.
6. Identificar o primeiro teste que falha — **esse** será o primeiro experimento.
7. Investigar com `rea-researcher` (Win7) e `nt61-architect` (ReactOS).
8. Escrever acceptance criteria e teste que falha hoje.
9. Implementar, revisar, gate, reportar.

---

## 8. Conclusão

**Não há primeiro experimento selecionado ainda.** E isso é o resultado correto desta fase: a honestidade sobre o estado do projeto é mais valiosa do que uma escolha prematura.

A próxima ação real do projeto é **desbloquear a baseline de build** (Bison + RAM). Até lá, todo o trabalho de "compatibilidade" seria especulação.