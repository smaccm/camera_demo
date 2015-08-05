.PHONY: demo
demo:
	make -C src $@
	cp src/$@ $@

.PHONY: vchan_test
vchan_test:
	make -C src $@
	cp src/$@ $@

.PHONY: clean
clean:
	make -C src $@
	rm -f demo vchan_test
