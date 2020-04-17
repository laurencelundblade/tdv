make -f Makefile.ossl clean
make -f tdv/Makefile.ossl.pedantic
make -f Makefile.ossl clean
make -f tdv/Makefile.ossl.gcc.pedantic
make -f Makefile.ossl clean
make -f Makefile.test
./t_cose_test
make -f Makefile.test clean
make -f Makefile.psa
./t_cose_test
make -f Makefile.psa clean
make -f Makefile.ossl
./t_cose_test
