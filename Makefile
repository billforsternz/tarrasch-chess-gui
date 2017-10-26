SRCDIR := src

.PHONY: srccode

srccode:
	$(MAKE) -C $(SRCDIR)

clean:
	rm -f $(SRCDIR)/*.o tarrasch
