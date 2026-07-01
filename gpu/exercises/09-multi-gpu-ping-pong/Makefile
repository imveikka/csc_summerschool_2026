# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

# LUMI-G

# Puhti
HIPCC = CC
LDFLAGS = -xhip -O3

ping-pong: ping-pong.cpp
	$(HIPCC) $(LDFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f ping-pong
