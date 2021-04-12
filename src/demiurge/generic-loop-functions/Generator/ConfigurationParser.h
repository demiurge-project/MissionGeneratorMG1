/*
 * @file <src/Base/BaseLoopFunctions.h>
 *
 * @author Antoine Ligot - <aligot@ulb.ac.be>
 *
 * @package ARGoS3-MissionGenerator
 *
 * @license MIT License
 */

#ifndef CONFIGURATION_PARSER
#define CONFIGURATION_PARSER

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <regex>
#include <numeric>

class ConfigurationParser {

  public:

    ConfigurationParser();

    virtual ~ConfigurationParser();

    struct SConfigVariable {
  
      std::string Name;

      std::string Label;

      std::string Type;

      std::vector<std::string> Range;

      std::vector<float> Distribution;
    
      std::vector<std::string> Condition;

      /**
        * Empty constructor
        */
      SConfigVariable() :
          Name("NA"), Label("NA"), Type("NA"), Range(std::vector<std::string>()), Distribution(std::vector<float>()), Condition(std::vector<std::string>()) {}

    };

    typedef std::vector<SConfigVariable> TConfiguration;

    TConfiguration& Parse(std::string s_config_file);


  private:

    std::vector<std::string> ParseRange(std::string str_range);
    std::vector<float> ParseDistribution(std::string str_distribution);
    std::vector<std::string> ParseCondition(std::string str_condition);

    /**
     * Checks if number of distribution values correspond to number of possible values in range, and if distribution values sum to 1.0
     */
    bool IsVariableCorrect(SConfigVariable s_variable);

    TConfiguration m_tConfiguration;
};

/**
 * Redefine the "<<" operator with one variable.
 */
std::ostream& operator<<(std::ostream& c_os,
                         const ConfigurationParser::SConfigVariable& s_config_variable);

/**
 * Redefine the "<<" operator with variables.
 */
std::ostream& operator<<(std::ostream& c_os,
                         const ConfigurationParser::TConfiguration& t_configuration);

std::vector<std::string> SplitString(const std::string &s_string, char delim);

#endif
