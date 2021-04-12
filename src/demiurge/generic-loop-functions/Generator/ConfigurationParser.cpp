#include "ConfigurationParser.h"


ConfigurationParser::ConfigurationParser() {}

/****************************************/
/****************************************/

ConfigurationParser::~ConfigurationParser() {}

/****************************************/
/****************************************/

ConfigurationParser::TConfiguration& ConfigurationParser::Parse(std::string s_config_file) {
  std::ifstream cConfigFileContent(s_config_file);

  std::string strLine;

  std::regex cMultiSpace ("[' ']{2,}");   // matches multiple spaces

  while(std::getline(cConfigFileContent, strLine)) {
    if (strLine.rfind('#', 0) == 0) {
      // Line is a comment, ignore it
    } else {
      ConfigurationParser::SConfigVariable sCurrentVariable;
      std::string strVariableDescription = SplitString(strLine, '|')[0];
      std::string strVariableCondition = SplitString(strLine, '|')[1];
      std::vector<std::string> vecVariableDescription = SplitString(std::regex_replace(strVariableDescription, cMultiSpace, " "), ' ');
      sCurrentVariable.Name = vecVariableDescription[0];
      sCurrentVariable.Label = vecVariableDescription[1];
      sCurrentVariable.Type = vecVariableDescription[2];
      sCurrentVariable.Range = ParseRange(vecVariableDescription[3]);
      if (vecVariableDescription.size() == 5) {
        sCurrentVariable.Distribution = ParseDistribution(vecVariableDescription[4]);
      }
      sCurrentVariable.Condition = ParseCondition(strVariableCondition);
      if (IsVariableCorrect(sCurrentVariable)) {
        m_tConfiguration.push_back(sCurrentVariable);
      } else {
        std::cout << "\033[1;31m" << "Error: variable \"" << sCurrentVariable.Name << "\" not correctly specified." << "\033[0m" << std::endl;
        std::exit(1);
      }
    }
  }

  return m_tConfiguration;

}

/****************************************/
/****************************************/

std::vector<std::string> ConfigurationParser::ParseRange(std::string str_range) {
  std::vector<std::string> vecRange;
  std::regex rWords ("\'([^\']*)\'");
  std::regex rNumbers (R"([0-9]*\.[0-9]+|[0-9]+)");
  std::smatch cSmatch;
  if (std::regex_search(str_range, cSmatch, rWords)) {
    for(std::sregex_iterator i = std::sregex_iterator(str_range.begin(), str_range.end(), rWords); i != std::sregex_iterator(); ++i) {
       cSmatch = *i;
       vecRange.push_back(cSmatch.str());
    }
  }
  else if (std::regex_search(str_range, cSmatch, rNumbers)) {
    for(std::sregex_iterator i = std::sregex_iterator(str_range.begin(), str_range.end(), rNumbers); i != std::sregex_iterator(); ++i) {
       cSmatch = *i;
       vecRange.push_back(cSmatch.str());
    }
  }
  return vecRange;
}

/****************************************/
/****************************************/

std::vector<float> ConfigurationParser::ParseDistribution(std::string str_distribution) {
  std::vector<float> vecDistribution;
  std::regex rNumbers (R"([0-9]*\.[0-9]+|[0-9]+)");
  std::smatch cSmatch;
  if (std::regex_search(str_distribution, cSmatch, rNumbers)) {
    for(std::sregex_iterator i = std::sregex_iterator(str_distribution.begin(), str_distribution.end(), rNumbers); i != std::sregex_iterator(); ++i) {
       cSmatch = *i;
       std::string strValue = cSmatch.str();
       vecDistribution.push_back(strtof(strValue.c_str(), 0));
    }
  }
  return vecDistribution;
}

/****************************************/
/****************************************/

std::vector<std::string> ConfigurationParser::ParseCondition(std::string str_condition) {
  std::vector<std::string> vecCondition;
  // If NA not in condition, process the string
  if (str_condition.find("NA") == std::string::npos) {
    vecCondition = SplitString(str_condition, '&');
    for (size_t i = 0; i < vecCondition.size(); i++) {
      // Removing extra spaces
      vecCondition[i] = std::regex_replace(vecCondition[i], std::regex("^ +| +$|( ) +"), "$1");
    }
  }
  return vecCondition;
}

/****************************************/
/****************************************/

bool ConfigurationParser::IsVariableCorrect(ConfigurationParser::SConfigVariable s_variable) {
  bool bCorrect = true;
  if (s_variable.Distribution.size() > 0) {
    if (s_variable.Range.size() != s_variable.Distribution.size()) {
      bCorrect = false;
    } else {
      float fSumValues = std::accumulate(s_variable.Distribution.begin(), s_variable.Distribution.end(), 0.0f);
      if (fSumValues != 1.0f) {
        bCorrect = false;
      }
    }
  }
  return bCorrect;
}

/****************************************/
/****************************************/

std::ostream& operator<<(std::ostream& c_os,
                         const ConfigurationParser::SConfigVariable& s_config_variable) {
  c_os << "SConfigVariable("
      << s_config_variable.Name << ", "
      << s_config_variable.Label << ", "
      << s_config_variable.Type << ", [";
  for (size_t i = 0; i < s_config_variable.Range.size(); ++i) {
   c_os << s_config_variable.Range[i];
   if (i < s_config_variable.Range.size()-1) {
     c_os << ", ";
   }
  }
  c_os << "], [";
  for (size_t i = 0; i < s_config_variable.Distribution.size(); ++i) {
   c_os << s_config_variable.Distribution[i];
   if (i < s_config_variable.Distribution.size()-1) {
     c_os << ", ";
   }
  }
  c_os << "], {";
  for (size_t i = 0; i < s_config_variable.Condition.size(); ++i) {
   c_os << s_config_variable.Condition[i];
   if (i < s_config_variable.Condition.size()-1) {
     c_os << ", ";
   }
  }
  c_os << "})";
  return c_os;
}

/****************************************/
/****************************************/

std::ostream& operator<<(std::ostream& c_os,
                         const ConfigurationParser::TConfiguration& t_configuration) {
   if(! t_configuration.empty()) {
      c_os << t_configuration[0];
      for(unsigned int i = 1; i < t_configuration.size(); ++i) {
         c_os << "\n" << t_configuration[i];
      }
   }
   return c_os;
}

/****************************************/
/****************************************/

std::vector<std::string> SplitString(const std::string &s_string, char delim) {
  std::vector<std::string> elems;
  std::stringstream ss(s_string);
  std::string elem;
  while(std::getline(ss, elem, delim)) {
     elems.push_back(elem);
  }
  return elems;
}
