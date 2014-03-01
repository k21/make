f: a.dummy c.dummy
	touch f

f: b.dummy

e: a.dummy

a: b d
	touch a

d: e
d: f
	touch d

b:
	touch b
