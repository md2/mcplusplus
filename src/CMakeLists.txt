SET(SOURCES
  AlgoCollection.cxx
  Base16.cxx
  BruteforceCrack.cxx
  Charset.cxx
  Db.cxx
  DictionaryCrack.cxx
  ExternalHasher.cxx
  HelperFunctions.cxx
  Loader.cxx
  main.cxx
  MD5.cxx
  RandomCrack.cxx
  RegexBfCrack.cxx
  RNG.cxx
  RuleApplicator.cxx
  RuleList.cxx
  RulePreprocessor.cxx
  SignalHandler.cxx
  TTY.cxx
  TwoDictionaryCrack.cxx)
ADD_EXECUTABLE (mc++ ${SOURCES})
IF(USE_INTERNAL_MD5)
  INCLUDE_DIRECTORIES(../lib)
  SET(LIBS md5)
ENDIF(USE_INTERNAL_MD5)
IF(HAVE_OPENSSL)
  LIST(APPEND LIBS crypto)
ENDIF(HAVE_OPENSSL)
LIST(APPEND LIBS sqlite3 popt)
TARGET_LINK_LIBRARIES(mc++ ${LIBS})
INSTALL(TARGETS mc++ DESTINATION bin)

