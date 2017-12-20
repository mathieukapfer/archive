
bin_PROGRAMS = testJitter testsmtp  testPing testmail testObserver #testAes

if ENABLE_OPENSSL
LD_OPENSSL = $(COMPONENT_PATH)/lib/libssl.so \
             $(COMPONENT_PATH)/lib/libcrypto.so
endif

COMPONENT_PATH = $(abs_top_srcdir)/../components/_$(target_cpu)
LD_COMPONENT =   \
	$(COMPONENT_PATH)/lib/libcurl.so \
        $(LD_OPENSSL)


testJitter_LDADD =  ../libUtil.a 	../../Framework/log/libTraceDebug.a
testJitter_SOURCES = testJitter.cpp
testJitter_LDFLAGS = -lrt

testsmtp_LDADD =  $(LD_COMPONENT) ../../Framework/log/libTraceDebug.a
testsmtp_SOURCES = smtp-tls.cpp

testmail_LDADD =  $(LD_COMPONENT) ../../Framework/log/libTraceDebug.a
testmail_SOURCES  = ../SendMail.h testSendMail.cpp

#testAes_LDADD =  $(LD_COMPONENT)  ../../Framework/log/libTraceDebug.a
#testAes_SOURCES  = ../AesAlgo.cpp ../AesAlgo.h testAes.cpp

testPing_LDADD =  $(LD_COMPONENT)  ../../Framework/log/libTraceDebug.a
testPing_SOURCES  = ../PingUtils.cpp ../PingUtils.h testPing.cpp

testObserver_LDADD =  $(LD_COMPONENT)  ../../Framework/log/libTraceDebug.a
testObserver_SOURCES  = ../Observer.h ../Observable.h testObserver.cpp

#./testsem;
#./testsem_posix;
test:testJitter
#	LD_LIBRARY_PATH=$(abs_top_srcdir)/../components/_i686/lib/  ./testmail ;  LD_LIBRARY_PATH=$(abs_top_srcdir)/../components/_i686/lib/  ./testAes | grep TEST
	LD_LIBRARY_PATH=$(abs_top_srcdir)/../components/_i686/lib/  ./testJitter ;

tftp:
	cp $(bin_PROGRAMS) /tftpboot
