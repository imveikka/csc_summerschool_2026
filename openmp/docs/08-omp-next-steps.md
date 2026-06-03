<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Next steps with OpenMP
event:  CSC Summer School in High-Performance Computing 2026
lang:   en
---

# Outline

- Next steps with OpenMP

# Next steps {.section}

# OpenMP programming best practices

- Explicitly declare variable privacy level (`shared`/`private`/`firstprivate`)
  - Avoid `shared` variables if not explicitly needed
  - *Default is `shared`!*
- Maximise parallel regions
  - Reduce fork-join overhead, e.g. combine multiple parallel loops into one large parallel region
  - Potential for better cache re-usage
- Parallelise outermost loops if possible
  - Move PARALLEL DO construct outside of inner loops
- If using tasks: ensure all threads have a task to work on

# Things that we did not cover

- sections construct
- scheduling clauses of `for`/`do` constructs
- taskgroup and taskloop constructs
- simd construct
- ...
