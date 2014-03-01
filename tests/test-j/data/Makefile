a1:
	@sleep 1
	touch $@

a2: a1
	@sleep 2
	touch $@

a3: a2
	@sleep 2
	touch $@

b1:
	@sleep 2
	touch $@

b2: b1
	@sleep 2
	touch $@

b3: b2
	@sleep 2
	touch $@

c1:
	@sleep 7
	touch $@

d: a3 b3 c1
	touch d
