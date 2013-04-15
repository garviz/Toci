libincludedir=$(includedir)/libfid

EXTRA_DIST=parserfuns.templ.c sequences.templ.c suffixarray.templ.c suffixarrayext.templ.c sequences.in.h suffixarray.in.h generic.fun parserfuns.fun sequences.fun suffixarray.fun

LIBHEADERS=libfidinttypes.h memory.h error.h options.h arrays.h alphabet.h dbfiles.h fileutils.h utilities.h sequences.h projectfile.h suffixarray.h suffixarrayext.h skiptree.h

lib_LTLIBRARIES=libfid.la

libfid_la_SOURCES=libdefs.h $(LIBHEADERS) alphabet.c check32in64.h\
 createfiles.c createfiles.h dbfiles.c dbfiles.h error.c\
 filereader.c filereader.h fileutils.c memory.c options.c\
 projectfile.c projectfileparser.h sequences.c\
 suffixarray.c suffixarrayext.c touint.h utilities.c vacopy.h verify.h

libfid_la_LDFLAGS=-export-symbols-regex="^fid_.*" -version-info @libfid_version_info@

libfid_la_LIBADD=$(LIBFID_LIBS)

# Generate library header for use with external programs
BUILT_SOURCES=parserfuns.48 sequences.h suffixarray.h suffixarrayext.48 libfid.h libfidxx.h

nodist_libinclude_HEADERS=libfid.h libfid.32 libfid.64 libfid.undef libfidxx.h

sequences.h: sequences.48
sequences.32: sequences.48
sequences.64: sequences.48
sequences.undef: sequences.48

sequences.48: $(srcdir)/sequences.in.h $(srcdir)/generic.fun $(srcdir)/sequences.fun
	rm -f $@
	$(GEN48) -h $(srcdir)/sequences.in.h -o sequences -s $(srcdir)/generic.fun -s $(srcdir)/sequences.fun -n
	$(GEN48) -A $(srcdir)/generic.fun -A $(srcdir)/sequences.fun -o sequences
	touch $@

parserfuns.32: parserfuns.48
parserfuns.64: parserfuns.48
parserfuns.undef: parserfuns.48

parserfuns.48: $(srcdir)/parserfuns.fun
	rm -f $@
	$(GEN48) -A $(srcdir)/parserfuns.fun -o parserfuns
	touch $@

suffixarray.h: suffixarray.48
suffixarray.32: suffixarray.48
suffixarray.64: suffixarray.48
suffixarray.undef: suffixarray.48

suffixarray.48: $(srcdir)/suffixarray.in.h $(srcdir)/generic.fun $(srcdir)/suffixarray.fun
	 rm -f $@
	$(GEN48) -h $(srcdir)/suffixarray.in.h -o suffixarray -s $(srcdir)/generic.fun -s $(srcdir)/suffixarray.fun -n
	$(GEN48) -A $(srcdir)/generic.fun -A $(srcdir)/suffixarray.fun -o suffixarray
	touch $@

suffixarrayext.32: suffixarrayext.48
suffixarrayext.64: suffixarrayext.48
suffixarrayext.undef: suffixarrayext.48

suffixarrayext.48: $(srcdir)/generic.fun
	rm -f $@
	$(GEN48) -A $(srcdir)/generic.fun -o suffixarrayext -n
	$(GEN48) -a sanity_checks -o suffixarrayext
	$(GEN48) -a init_from_esa -o suffixarrayext
	touch $@

libfid.32: libfid.48
libfid.64: libfid.48
libfid.undef: libfid.48

libfid.48: $(srcdir)/sequences.in.h $(srcdir)/suffixarray.in.h $(srcdir)/generic.fun $(srcdir)/sequences.fun $(srcdir)/suffixarray.fun
	rm -f $@
	$(GEN48) -h $(srcdir)/sequences.in.h -o libfid_temp -s $(srcdir)/generic.fun -s $(srcdir)/sequences.fun -n -t
	$(GEN48) -a fid_Sequenceiterfun -o libfid_temp
	$(GEN48) -a fid_SEQUENCE_DESCRIPTION -o libfid_temp
	$(GEN48) -a fid_SEQUENCE_DESCRIPTION_LENGTH -o libfid_temp
	$(GEN48) -h $(srcdir)/suffixarray.in.h -o libfid_temp -s $(srcdir)/generic.fun -s $(srcdir)/suffixarray.fun -t
	$(GEN48) -a fid_LCP -o libfid_temp
	$(GEN48) -a fid_Suffixinterval -o libfid_temp
	$(GEN48) -a fid_Esatraversecallback -o libfid_temp
	$(GEN48) -a fid_SUFFIXINTERVAL_SINGLETON -o libfid_temp
	$(GEN48) -a fid_suffixinterval_init_root -o libfid_temp
	$(GEN48) -A $(srcdir)/generic.fun -o libfid_temp
	mv libfid_temp.32 libfid.32
	mv libfid_temp.64 libfid.64
	mv libfid_temp.undef libfid.undef
	rm libfid_temp.h
	touch $@

libfid.h: Makefile $(LIBHEADERS)
	$(BUILDHEADER) $(srcdir) $(LIBHEADERS) > libfid.h

libfidxx.h: libfid.48
	rm -f $@
	$(srcdir)/../scripts/buildtraits.sh libfid_temp.t32 libfid_temp.t64 > $@
	rm libfid_temp.t32 libfid_temp.t64

# Splint rules
SPLINTFILES=alphabet.splint createfiles.splint dbfiles.splint error.splint\
 filereader.splint fileutils.splint memory.splint options.splint\
 projectfile.splint sequences.splint suffixarray.splint suffixarrayext.splint\
 utilities.splint

include $(top_srcdir)/fragments/splintrules.make

CLEANFILES=$(SPLINTFILES) libfid.h libfid.32 libfid.48 libfid.64 libfid.undef libfidxx.h parserfuns.32 parserfuns.48 parserfuns.64 parserfuns.undef sequences.h sequences.32 sequences.48 sequences.64 sequences.undef suffixarray.h suffixarray.32 suffixarray.48 suffixarray.64 suffixarray.undef suffixarrayext.32 suffixarrayext.48 suffixarrayext.64 suffixarrayext.undef
