#
# Makefile for libtsplite/XX
#

SHELL = /bin/sh

dist_dir = ../..

dist_libdir = $(dist_dir)/lib
dist_includedir = $(dist_dir)/include

COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)
ARFLAGS = rUv

LIB = $(dist_libdir)/libtsplite.a

.PHONY: mostlyclean clean

.PRECIOUS: $(LIB)
$(LIB): \
	$(LIB)(AFclose.o) \
	$(LIB)(AFdReadData.o) \
	$(LIB)(AFdWriteData.o) \
	$(LIB)(AFopnRead.o) \
	$(LIB)(AFopnWrite.o) \
	$(LIB)(AFprintChunkLims.o) \
	$(LIB)(AFprintInfoRecs.o) \
	$(LIB)(AFsetFileType.o) \
	$(LIB)(AFsetInfo.o) \
	$(LIB)(AFsetInputPar.o) \
	$(LIB)(AFsetSpeaker.o) \
	$(LIB)(dataio/AFdRdFx.o) \
	$(LIB)(dataio/AFdRdIx.o) \
	$(LIB)(dataio/AFdRdTA.o) \
	$(LIB)(dataio/AFdRdXlaw.o) \
	$(LIB)(dataio/AFdWrFx.o) \
	$(LIB)(dataio/AFdWrIx.o) \
	$(LIB)(dataio/AFdWrTA.o) \
	$(LIB)(dataio/AFdWrXlaw.o) \
	$(LIB)(dataio/AFgetLine.o) \
	$(LIB)(dataio/AFposition.o) \
	$(LIB)(dataio/AFseek.o) \
	$(LIB)(dataio/AFtell.o) \
	$(LIB)(deprecated/AFfReadData.o) \
	$(LIB)(deprecated/AFfWriteData.o) \
	$(LIB)(deprecated/AFopenRead.o) \
	$(LIB)(deprecated/AFopenWrite.o) \
	$(LIB)(deprecated/AFreadData.o) \
	$(LIB)(deprecated/AFwriteData.o) \
	$(LIB)(deprecated/dataio/AFfRdFx.o) \
	$(LIB)(deprecated/dataio/AFfRdIx.o) \
	$(LIB)(deprecated/dataio/AFfRdTA.o) \
	$(LIB)(deprecated/dataio/AFfRdXlaw.o) \
	$(LIB)(deprecated/dataio/AFfWrFx.o) \
	$(LIB)(deprecated/dataio/AFfWrIx.o) \
	$(LIB)(deprecated/dataio/AFfWrTA.o) \
	$(LIB)(deprecated/dataio/AFfWrXlaw.o) \
	$(LIB)(header/AFfindFtype.o) \
	$(LIB)(header/AFrdAIhead.o) \
	$(LIB)(header/AFrdAUhead.o) \
	$(LIB)(header/AFrdBLhead.o) \
	$(LIB)(header/AFrdEShead.o) \
	$(LIB)(header/AFrdInfoText.o) \
	$(LIB)(header/AFrdINhead.o) \
	$(LIB)(header/AFrdNShead.o) \
	$(LIB)(header/AFrdSFhead.o) \
	$(LIB)(header/AFrdSPhead.o) \
	$(LIB)(header/AFrdSWhead.o) \
	$(LIB)(header/AFrdTAhead.o) \
	$(LIB)(header/AFrdWVhead.o) \
	$(LIB)(header/AFreadHead.o) \
	$(LIB)(header/AFsetChunkLim.o) \
	$(LIB)(header/AFsetNHread.o) \
	$(LIB)(header/AFsetNHwrite.o) \
	$(LIB)(header/AFupdAIhead.o) \
	$(LIB)(header/AFupdAUhead.o) \
	$(LIB)(header/AFupdHead.o) \
	$(LIB)(header/AFupdWVhead.o) \
	$(LIB)(header/AFwrAIhead.o) \
	$(LIB)(header/AFwrAUhead.o) \
	$(LIB)(header/AFwriteHead.o) \
	$(LIB)(header/AFwrTAhead.o) \
	$(LIB)(header/AFwrWVhead.o) \
	$(LIB)(info/AFaddInfoChunk.o) \
	$(LIB)(info/AFaddInfoRec.o) \
	$(LIB)(info/AFdecInfoVVU.o) \
	$(LIB)(info/AFdelInfoRec.o) \
	$(LIB)(info/AFgenSpkrConfig.o) \
	$(LIB)(info/AFgenSpkrNames.o) \
	$(LIB)(info/AFgetDelInfoRec.o) \
	$(LIB)(info/AFgetInfoBytes.o) \
	$(LIB)(info/AFgetInfoChan.o) \
	$(LIB)(info/AFgetInfoFrame.o) \
	$(LIB)(info/AFgetInfoFullScale.o) \
	$(LIB)(info/AFgetInfoNbS.o) \
	$(LIB)(info/AFgetInfoRec.o) \
	$(LIB)(info/AFgetInfoSfreq.o) \
	$(LIB)(info/AFgetInfoSpkr.o) \
	$(LIB)(info/AFgetInfoSwap.o) \
	$(LIB)(info/AFspkrConfigMask.o) \
	$(LIB)(nucleus/AFcheckDataPar.o) \
	$(LIB)(nucleus/AFoptions.o) \
	$(LIB)(nucleus/AFpresetWPar.o) \
	$(LIB)(nucleus/AFprintAFpar.o) \
	$(LIB)(nucleus/AFsetRpar.o) \
	$(LIB)(nucleus/AFsetWpar.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o */*.o

clean: mostlyclean
