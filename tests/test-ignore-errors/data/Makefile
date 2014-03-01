f: a.dummy c.dummy
	touch f

f: b.dummy

e: a.dummy
	true
	touch e
	-false

a: b d
	-false
	@-false
	-@false
	touch a

d: e
d: f
	touch d

b:
	touch b
