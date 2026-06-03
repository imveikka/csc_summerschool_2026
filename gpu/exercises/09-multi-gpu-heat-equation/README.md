<!--
SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Multi-GPU heat equation with HIP

Port the evolve code in the `core.cpp` to a GPU.

To do that, you need to also manage GPU memory either explicitly or using HIP
managed memory.

For explicit memory management consider

- Duplicating previous and current fields to GPU and initializing them in
  initialize() setup.cpp 
- Freeing GPU memory differently
- Copying the GPU memory back everytime writing the image to disk
