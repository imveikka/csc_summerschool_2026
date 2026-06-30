# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

# LUMI-G

<<<<<<< HEAD
# Puhti
HIPCC = CC
LDFLAGS = -xhip -O3

ping-pong: ping-pong.cpp
	$(HIPCC) $(LDFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f ping-pong
=======
ping-pong: ping-pong.cpp
	CC -xhip ping-pong.cpp -o ping-pong

>>>>>>> 5655df11 (multi-gpu-ping-pong Makefile LUMIfied)
