#include "AlgoCollection.h"
#include "Base16.h"
#include "BruteforceCrack.h"
#include "Charset.h"
#include "Db.h"
#include "DictionaryCrack.h"
#include "ExternalHasher.h"
#include "HelperFunctions.h"
#include "i18n.h"
#include "Loader.h"
#include "MD5.h"
#include "RandomCrack.h"
#include "RegexBfCrack.h"
#include "RNG.h"
#include "RuleApplicator.h"
#include "RuleList.h"
#include "RulePreprocessor.h"
#include "SignalHandler.h"
#include "TTY.h"
#include "TwoDictionaryCrack.h"
#include "stringlist.h"
#include <string>
#include <stdio.h>

extern char *output;

