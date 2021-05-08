


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 956213

  The file contains the Areas class implementation. Areas are the top
  level of the data structure in Beth Yw? for now.

  Areas is also responsible for importing data from a stream (using the
  various populate() functions) and creating the Area and Measure objects.

  This file contains numerous functions you must implement. Each function you
  must implement has a TODO block comment. 
*/

#include <stdexcept>
#include <iostream>
#include <string>
#include <tuple>
#include <sstream>
#include <queue>

#include "lib_json.hpp"

#include "datasets.h"
#include "areas.h"

/*
  An alias for the imported JSON parsing library.
*/
using json = nlohmann::json;

/*
  TODO: Areas::Areas()

  Constructor for an Areas object.

  @example
    Areas data = Areas();
*/
Areas::Areas() = default;

/*
  TODO: Areas::setArea(localAuthorityCode, area)

  Add a particular Area to the Areas object.

  If an Area already exists with the same local authority code, overwrite all
  data contained within the existing Area with those in the new
  Area (i.e. they should be combined, but the new Area's data should take
  precedence, e.g. replace a name with the same language identifier).

  @param localAuthorityCode
    The local authority code of the Area

  @param area
    The Area object that will contain the Measure objects

  @return
    void

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
*/
void Areas::setArea(const std::string& localAuthorityCode, Area area) {
    if (this->areasContainer.find(localAuthorityCode) == this->areasContainer.end()) {
        //doesn't exist
        this->areasContainer.insert(std::pair<std::string, Area>(localAuthorityCode, area));
    } else {
        //exists
        //update names
        for (auto & record : area.getNames()) {
            this->getArea(localAuthorityCode).setName(record.first, record.second);
        }

        //update measures
        for (auto & record : area.getMeasures()) {
            this->getArea(localAuthorityCode).setMeasure(record.second.getCodename(), record.second);
        }
    }
}

/*
  TODO: Areas::getArea(localAuthorityCode)

  Retrieve an Area instance with a given local authority code.

  @param localAuthorityCode
    The local authority code to find the Area instance of

  @return
    An Area object

  @throws
    std::out_of_range if an Area with the set local authority code does not
    exist in this Areas instance

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
    ...
    Area area2 = areas.getArea("W06000023");
*/

Area& Areas::getArea(const std::string& localAuthorityCode) {
    if (this->areasContainer.find(localAuthorityCode) != this->areasContainer.end()) {
        //does exist
        return this->areasContainer.at(localAuthorityCode);
    }
    throw std::out_of_range("No area found matching " + localAuthorityCode);
}


/*
  TODO: Areas::size()

  Retrieve the number of Areas within the container. This function should be 
  callable from a constant context, not modify the state of the instance, and
  must promise not throw an exception.

  @return
    The number of Area instances

  @example
    Areas data = Areas();
    std::string localAuthorityCode = "W06000023";
    Area area(localAuthorityCode);
    data.setArea(localAuthorityCode, area);
    
    auto size = areas.size(); // returns 1
*/
unsigned int Areas::size() {
    return this->areasContainer.size();
}

/*
  TODO: Areas::populateFromAuthorityCodeCSV(is, cols, areasFilter)

  This function specifically parses the compiled areas.csv file of local 
  authority codes, and their names in English and Welsh.

  This is a simple dataset that is a comma-separated values file (CSV), where
  the first row gives the name of the columns, and then each row is a set of
  data.

  For this coursework, you can assume that areas.csv will always have the same
  three columns in the same order.

  Once the data is parsed, you need to create the appropriate Area objects and
  insert them in to a Standard Library container within Areas.

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @return
    void

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/areas.csv");
    auto is = input.open();

    auto cols = InputFiles::AREAS.COLS;

    auto areasFilter = BethYw::parseAreasArg();

    Areas data = Areas();
    areas.populateFromAuthorityCodeCSV(is, cols, &areasFilter);

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromAuthorityCodeCSV(
    std::istream &is,
    const BethYw::SourceColumnMapping &cols,
    const StringFilterSet * const areasFilter) {

    if(is.good()) {

        //read top line
        std::string line;
        std::string token;
        std::vector<std::string> tokens;

        std::getline(is, line);
        std::stringstream ss(line);

        //check the column headers are good
        while (std::getline(ss, token, ',')) {
            if (token == cols.at(BethYw::AUTH_CODE)
                || token == cols.at(BethYw::AUTH_NAME_ENG)
                || token == cols.at(BethYw::AUTH_NAME_CYM)) {
                tokens.push_back(token);
            } else {
                throw std::runtime_error("malformed file, unexpected column header");
            }
        }

        //check for the right number of column headers
        if (tokens.size() != 3) {
            throw std::out_of_range("Incorrect number of columns");
        }

        //read data
        while (std::getline(is, line)) {
            ss.clear();
            ss = std::stringstream(line);
            std::string auth_code;
            std::string name_eng;
            std::string name_cym;

            std::getline(ss, auth_code, ',');
            std::getline(ss, name_eng, ',');
            std::getline(ss, name_cym, ',');

            //check if line is valid
            if (!areasFilter
            || areasFilter->empty()
            || areasFilter->find(auth_code) != areasFilter->end()
            || areasFilter->find(name_eng) != areasFilter->end()
            || areasFilter->find(name_cym) != areasFilter->end()) {

                //create area
                Area area(auth_code);
                area.setName("eng", name_eng);
                area.setName("cym", name_cym);
                setArea(auth_code, area);
            }

        }
    } else {
        throw std::runtime_error("Parsing error with areas.csv");
    }

}

/*
  TODO: Areas::populateFromWelshStatsJSON(is,
                                          cols,
                                          areasFilter,
                                          measuresFilter,
                                          yearsFilter)

  Data from StatsWales is in the JSON format, and contains three
  top-level keys: odata.metadata, value, odata.nextLink. value contains the
  data we need. Rather than been hierarchical, it contains data as a
  continuous list (e.g. as you would find in a table). For each row in value,
  there is a mapping of various column headings and their respective vaues.

  Therefore, you need to go through the items in value (in a loop)
  using a JSON library. To help you, I've selected the nlohmann::json
  library that you must use for your coursework. Read up on how to use it here:
  https://github.com/nlohmann/json

  Example of using this library:
    - Reading/parsing in from a stream is very simply using the >> operator:
        json j;
        stream >> j;

    - Looping through parsed JSON is done with a simple for each loop. Inside
      the loop, you can access each using the array syntax, with the key/
      column name, e.g. data["Localauthority_ItemName_ENG"] gives you the
      local authority name:
        for (auto& el : j["value"].items()) {
           auto &data = el.value();
           std::string localAuthorityCode = data["Localauthority_ItemName_ENG"];
           // do stuff here...
        }

  In this function, you will have to parse the JSON datasets, extracting
  the local authority code, English name (the files only contain the English
  names), and each measure by year.

  If you encounter an Area that does not exist in the Areas container, you
  should create the Area object

  If areasFilter is a non-empty set only include areas matching the filter. If
  measuresFilter is a non-empty set only include measures matching the filter.
  If yearsFilter is not equal to <0,0>, only import years within the range
  specified by the tuple (inclusive).

  I've provided the column names for each JSON file that you need to parse
  as std::strings in datasets.h. This mapping should be passed through to the
  cols parameter of this function.

  Note that in the JSON format, years are stored as strings, but we need
  them as ints. When retrieving values from the JSON library, you will
  have to cast them to the right type.

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings of areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings of measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as the range of years to be imported (inclusively)

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    areas.populateFromWelshStatsJSON(
      is,
      cols,
      &areasFilter,
      &measuresFilter,
      &yearsFilter);
*/
void Areas::populateFromWelshStatsJSON(std::istream& is, const BethYw::SourceColumnMapping &cols,
                                       const StringFilterSet *const areasFilter,
                                       const StringFilterSet *const measuresFilter,
                                       const YearFilterTuple *const yearsFilter) {
    json j;
    is >> j;

    for (auto &el : j["value"].items()) {
        auto &data = el.value();

        //get what we need for area
        std::string name_eng = std::string();
        std::string name_cym = std::string();
        std::string auth_code = data[cols.at(BethYw::AUTH_CODE)];
        if (cols.find(BethYw::AUTH_NAME_ENG) != cols.end()) {
            name_eng = data[cols.at(BethYw::AUTH_NAME_ENG)];
        }
        if (cols.find(BethYw::AUTH_NAME_CYM) != cols.end()) {
            name_cym = data[cols.at(BethYw::AUTH_NAME_CYM)];
        }

        //check if null or empty or is in filter
        if (!areasFilter
            || areasFilter->empty()
            || areasFilter->find(auth_code) != areasFilter->end()
            || areasFilter->find(name_eng) != areasFilter->end()
            || areasFilter->find(name_cym) != areasFilter->end()) {
            //create area object
            Area area(auth_code);
            if (!name_eng.empty()) {
                area.setName("eng", name_eng);
            }
            if (!name_cym.empty()) {
                area.setName("cym", name_cym);
            }

            //get year
            unsigned int year = stoi(std::string(data[cols.at(BethYw::YEAR)]));
            //check if year is in range or range is empty
            if ((std::get<1>(*yearsFilter) >= year && year >= std::get<0>(*yearsFilter))
            || (std::get<0>(*yearsFilter) == 0 && std::get<1>(*yearsFilter) == 0)) {

                //get the values needed for measures
                std::string codename;
                std::string label;
                //deals with differing measure enums
                if (cols.find(BethYw::MEASURE_NAME) != cols.end()) {
                    codename = data[cols.at(BethYw::MEASURE_CODE)];
                    label = data[cols.at(BethYw::MEASURE_NAME)];
                } else {
                    codename = cols.at(BethYw::SINGLE_MEASURE_CODE);
                    label = cols.at(BethYw::SINGLE_MEASURE_NAME);
                }

                //gets code name as lower case
                std::transform(codename.begin(), codename.end(), codename.begin(), ::tolower);

                //gets the value whether it's saved as string or double
                double value = (data[cols.at(BethYw::VALUE)].type() == json::value_t::number_float) ?
                        (double) data[cols.at(BethYw::VALUE)] : std::stod(std::string(data[cols.at(BethYw::VALUE)]));

                //check if null or empty or in filter
                if (!measuresFilter
                    || measuresFilter->empty()
                    || measuresFilter->find(codename) != measuresFilter->end()
                    || measuresFilter->find(label) != measuresFilter->end()) {

                    //finally create measure with measure attributes, add to area object and add to container
                    Measure measure(codename, label);
                    measure.setValue(year, value);
                    area.setMeasure(codename, measure);
                    setArea(auth_code, area);
                }
            }
        }
    }
}

/*
  TODO: Areas::populateFromAuthorityByYearCSV(is,
                                              cols,
                                              areasFilter,
                                              yearFilter)

  This function imports CSV files that contain a single measure. The 
  CSV file consists of columns containing the authority code and years.
  Each row contains an authority code and values for each year (or no value
  if the data doesn't exist).

  Note that these files do not include the names for areas, instead you 
  have to rely on the names already populated through 
  Areas::populateFromAuthorityCodeCSV();

  The datasets that will be parsed by this function are
   - complete-popu1009-area.csv
   - complete-popu1009-pop.csv
   - complete-popu1009-opden.csv

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of strings for measures to import, or an empty 
    set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/complete-popu1009-pop.csv");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["complete-pop"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto yearsFilter = BethYw::parseYearsArg();

    Areas data = Areas();
    areas.populateFromAuthorityCodeCSV(is, cols, &areasFilter, &yearsFilter);

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromAuthorityByYearCSV(std::istream& is,
                                           const BethYw::SourceColumnMapping& cols,
                                           const StringFilterSet * const areasFilter,
                                           const YearFilterTuple * const yearsFilter) {
    //primary key is year
    //vector contains many <auth code, value> pairs
    std::map<unsigned int, std::vector<std::pair<std::string, double>>> data_table;
    if(is.good()) {
        //read top line
        std::string line;
        std::string token;

        std::getline(is, line);
        std::stringstream ss(line);

        //insert column headers as keys
        while (std::getline(ss, token, ',')) {
            //ignore auth code column header
            if (token != cols.at(BethYw::AUTH_CODE)) {
                    //insert keys and blank vectors
                    data_table.insert(std::pair<unsigned int, std::vector<std::pair<std::string, double>>>(stoi(token), std::vector<std::pair<std::string, double>>{}));
                }
            }


        //check for the right number of column headers (minus the auth code)
        if (data_table.size() != 11) {
            throw std::out_of_range("Malformed file: There is an incorrect number of columns");
        }

        //read data
        while(std::getline(is, line)) {
            ss.clear();
            ss = std::stringstream(line);

            //extract auth code
            std::string auth_code;
            std::getline(ss, auth_code, ',');

            //go through a line
            for (auto& elem : data_table) {
                //extract each value
                std::string value;
                std::getline(ss, value, ',');
                //push back a pair with extracted auth code
                elem.second.push_back(std::pair<std::string, double>(auth_code, stod(value)));
            }
        }
    }

    //deal with filters
    for (const auto& elem : data_table) {
        //if key (year) matches yearfilter range or yearfilter is <0,0>
        if (((elem.first >= std::get<0>(*yearsFilter) && elem.first <= std::get<1>(*yearsFilter))
                || ((std::get<0>(*yearsFilter) == 0 && std::get<1>(*yearsFilter) == 0)))) {
            //for each item in vector
            for (auto item : elem.second) {

                //if pair pos 0 (authcode) is in areasFilter or null
                if (!areasFilter
                || areasFilter->empty()
                || areasFilter->find(std::get<0>(item)) != areasFilter->end()) {
                    //make new area
                    Area area(std::get<0>(item));
                    Measure measure(cols.at(BethYw::SINGLE_MEASURE_CODE), cols.at(BethYw::SINGLE_MEASURE_NAME));
                    measure.setValue(elem.first, std::get<1>(item));
                    area.setMeasure(cols.at(BethYw::SINGLE_MEASURE_CODE), measure);

                    setArea(std::get<0>(item), area);
                }
            }
        }
    }
}


/*
  TODO: Areas::populate(is, type, cols)

  Parse data from an standard input stream `is`, that has data of a particular
  `type`, and with a given column mapping in `cols`.

  This function should look at the `type` and hand off to one of the three 
  functions populate………() functions.

  The function must check if the stream is in working order and has content.

  @param is
    The input stream from InputSource

  @param type
    A value from the BethYw::SourceDataType enum which states the underlying
    data file structure

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file),
    the stream is not open/valid/has any contents, or an unexpected type
    is passed in.
    std::out_of_range if there are not enough columns in cols

  @see
    See datasets.h for details of the values variable type can have

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    Areas data = Areas();
    areas.populate(
      is,
      DataType::WelshStatsJSON,
      cols);
*/
void Areas::populate(std::istream &is,
                     const BethYw::SourceDataType &type,
                     const BethYw::SourceColumnMapping &cols) {
  if (type == BethYw::AuthorityCodeCSV) {
    populateFromAuthorityCodeCSV(is, cols);
  } else if (type == BethYw::WelshStatsJSON) {
      populateFromWelshStatsJSON(is, cols, NULL, NULL, NULL);
  } else if (type == BethYw::AuthorityByYearCSV) {
      populateFromAuthorityByYearCSV(is, cols, NULL, NULL);
  } else {
      throw std::runtime_error("Areas::populate: Unexpected data type");
  }
}

/*
  TODO: Areas::populate(is,
                        type,
                        cols,
                        areasFilter,
                        measuresFilter,
                        yearsFilter)

  Parse data from an standard input stream, that is of a particular type,
  and with a given column mapping, filtering for specific areas, measures,
  and years, and fill the container.

  This function should look at the `type` and hand off to one of the three 
  functions you've implemented above.

  The function must check if the stream is in working order and has content.

  This overloaded function includes pointers to the three filters for areas,
  measures, and years.

  @param is
    The input stream from InputSource

  @param type
    A value from the BethYw::SourceDataType enum which states the underlying
    data file structure

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings for measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file),
    the stream is not open/valid/has any contents, or an unexpected type
    is passed in.
    std::out_of_range if there are not enough columns in cols

  @see
    See datasets.h for details of the values variable type can have

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variables areasFilter, measuresFilter,
    and yearsFilter are created

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    areas.populate(
      is,
      DataType::WelshStatsJSON,
      cols,
      &areasFilter,
      &measuresFilter,
      &yearsFilter);
*/
void Areas::populate(
    std::istream &is,
    const BethYw::SourceDataType &type,
    const BethYw::SourceColumnMapping &cols,
    const StringFilterSet * const areasFilter,
    const StringFilterSet * const measuresFilter,
    const YearFilterTuple * const yearsFilter)
     {
         if (type == BethYw::AuthorityCodeCSV) {
             populateFromAuthorityCodeCSV(is, cols, areasFilter);
         } else if (type == BethYw::WelshStatsJSON) {
             populateFromWelshStatsJSON(is, cols, areasFilter, measuresFilter, yearsFilter);
         } else if (type == BethYw::AuthorityByYearCSV) {
             //deal with measure filter here because it isn't passed in
             if (!measuresFilter
             || measuresFilter->empty()
             || measuresFilter->find(cols.at(BethYw::SINGLE_MEASURE_NAME)) != measuresFilter->end()
             || measuresFilter->find(cols.at(BethYw::SINGLE_MEASURE_CODE)) != measuresFilter->end()) {
                 populateFromAuthorityByYearCSV(is, cols, areasFilter, yearsFilter);
             }

         } else {
             throw std::runtime_error("Areas::populate: Unexpected data type");
         }
}

/*
  TODO: Areas::toJSON()

  Convert this Areas object, and all its containing Area instances, and
  the Measure instances within those, to values.

  Use the sample JSON library as above to create this. Construct a blank
  JSON object:
    json j;

  Convert this json object to a string:
    j.dump();

  You then need to loop through your areas, measures, and years/values
  adding this data to the JSON object.

  Read the documentation for how to convert your outcome code to JSON:
    https://github.com/nlohmann/json#arbitrary-types-conversions
  
  The JSON should be formatted as such:
    {
    "<localAuthorityCode1>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         …
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             …
                                            "<yearN>": <valueN> }
                               },
    "<localAuthorityCode2>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         …
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             …
                                            "<yearN>": <valueN> }
                               }
    ...
    "<localAuthorityCodeN>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         …
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             …
                                            "<yearN>": <valueN> }
                               }
    }

  An empty JSON is "{}" (without the quotes), which you must return if your
  Areas object is empty.
  
  @return
    std::string of JSON

  @example
    InputFile input("data/popu1009.json");
    auto is = input.open();

    auto cols = InputFiles::DATASETS["popden"].COLS;

    auto areasFilter = BethYw::parseAreasArg();
    auto measuresFilter = BethYw::parseMeasuresArg();
    auto yearsFilter = BethYw::parseMeasuresArg();

    Areas data = Areas();
    std::cout << data.toJSON();
*/
std::string Areas::toJSON() const {
  json j;

  //for every area
  for (auto area : getAreasContainer()) {
      //for every measure in the area
      for (auto measure : area.second.getMeasures()) {
          // for every value in the measure
          for (auto value : measure.second.getValues()) {
              //[area code][measures][codename][year] = [value]
              j[area.first]["measures"][measure.first][std::to_string(value.first)] = value.second;
          }
      }
      //for every name in the area
      for (auto name : area.second.getNames()) {
          //[area code][names][lang] = [name]
          j[area.first]["names"][name.first] = name.second;
      }
  }

  return j.dump() == "null" ? "{}" : j.dump();
}





/*
  TODO: operator<<(os, areas)

  Overload the << operator to print all of the imported data.

  Output should be formatted like the following to pass the tests. Areas should
  be printed, ordered alphabetically by their local authority code. Measures 
  within each Area should be ordered alphabetically by their codename.

  See the coursework specification for more information, although for reference
  here is a quick example of how output should be formatted:

    <English name of area 1> / <Welsh name of area 1> (<authority code 1>)
    <Measure 1 name> (<Measure 1 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>


    <English name of area 2> / <Welsh name of area 2> (<authority code 2>)
    <Measure 1 name> (<Measure 1 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x code>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>

    ...

    <English name of area y> / <Welsh name of area y> (<authority code y>)
    <Measure 1 name> (<Measure 1 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x codename>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>

  With real data, your output should start like this for the command
  bethyw --dir <dir> -p popden -y 1991-1993 (truncated for readability):

    Isle of Anglesey / Ynys Môn (W06000001)
    Land area (area) 
          1991       1992       1993    Average    Diff.  % Diff. 
    711.680100 711.680100 711.680100 711.680100 0.000000 0.000000 

    Population density (dens) 
         1991      1992      1993   Average    Diff.  % Diff. 
    97.126504 97.486216 98.038430 97.550383 0.911926 0.938905 

    Population (pop) 
            1991         1992         1993      Average      Diff.  % Diff. 
    69123.000000 69379.000000 69772.000000 69424.666667 649.000000 0.938906 


    Gwynedd / Gwynedd (W06000002)
    Land area (Area)
    ...

  @param os
    The output stream to write to

  @param areas
    The Areas object to write to the output stream

  @return
    Reference to the output stream

  @example
    Areas areas();
    std::cout << areas << std::end;
*/

std::ostream &operator<<(std::ostream &os, Areas &areas) {
    for (auto area : areas.getAreasContainer()) {
        os << area.second << std::endl;
    }
    return os;
}


/**
 * This function returns the entire map of areas owned by an Areas object
 *
 * @return
 *  A std::map object containing all the areas with their corresponding auth code
 */

const AreasContainer &Areas::getAreasContainer() const {
    return areasContainer;
}