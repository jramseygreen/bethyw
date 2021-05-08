# Advanced Object Oriented Programming Coursework 1
This coursework is the culmination of working with C++ and required us to create a comprehensive tool for data analysis. I feel as though this coursework in particular familiarised me with proper memory management and I was able to utilise Agile workflows to complete this alongside other work to a high standard, achieving 88%.

The task and framework which were provided to us are available [here](https://github.com/jramseygreen/bethyw/assignment).

###### __Name:__ Joshua Green
###### __Student Number:__ 956213
###### __Coursework:__ Beth Yw? StatsWales Data Analysis Tool



## Additional Functions

### Getter Functions

I used a few additional getter functions to aid sweeping checks / changes to all the members inside of an object instance. Below are all of the additional getters I created along with their documentation.

|  Class | Function name      | Parameters |                      Returns                            |
|  ----- | ------------------ | ---------- | ------------------------------------------------------- |
|  area  | getMeasures()      |     N/A    | **std::map** of measure objects with corresponding code |
|  area  | getNames()         |     N/A    | **std::map** of names with their 3 letter language code |
|  areas | getAreasContainer()|     N/A    | **std::map** of areas with their authority code         |
| measure| getValues()        |     N/A    | **std::map** of all values with the corresponding year  |

#### 1. area.getMeasures()

```c++
/**
 * This function returns the entire map of measures owned by an area object
 *
 * @return
 *  A std::map object containing all the measures with their corresponding code name
 */

std::map<std::string, Measure> & Area::getMeasures()  {
return this->measures;
}
```

#### 2. area.getNames()

```c++
/**
 * This function returns the entire map of names owned by an area object
 *
 * @return
 *  A std::map object containing all the names with their corresponding 3 letter language code
 */

std::map<std::string, std::string> & Area::getNames() {
    return this->names;
}
```

#### 3. areas.getAreasContainer()

```c++
/**
 * This function returns the entire map of areas owned by an Areas object
 *
 * @return
 *  A std::map object containing all the areas with their corresponding auth code
 */

const AreasContainer &Areas::getAreasContainer() const {
    return areasContainer;
}
```

#### 4. measure.getValues()

```c++
/**
 * This function returns the entire map of values owned by a measure object
 *
 * @return
 *  A std::map object containing all the values with their corresponding year
 */
std::map<unsigned int, double> Measure::getValues() {
    return this->values;
}
```

### Other Functions
|  Class | Function name      | Parameters                   |              Returns                       |
|  ----- | ------------------ | ---------------------------- | ------------------------------------------ |
| measure| alignValue()       | **T** t, **const int** width | **std::string** t aligned right & padded   |

#### 1. measure.alignValue

```c++
/**
 * This will return a padded and aligned value for the ostream.
 *
 * @param t - the value to align & pad
 * @param width - The maximum length of a column
 * @return - std::string aligned right and padded
 */
template<typename T>
std::string Measure::alignValue(T t, const int &width) {
    std::stringstream stream;
    stream << std::right << std::setw(width) << std::setfill(' ') << t;
    return stream.str();
}
```


## Overall Logic Description

### Container choices

I chose to use maps (**std::map**) for everything due to needing to order the output apropriately (dealt with by maps already) and also because they are fast and easy to handle when it comes to structuring for JSON dumping.

### Algorithms For Reading Data From Files

I implemented similar logic for each of the populateFrom_____ functions, in that I typically used one stage of logic for each filter, per line, to determine whether that line should have its data imported.

This also meant that I consider the filter being *'all', 'null' or empty* the same as having found a match inside the filter for the given line we are currently parsing in the file.

__Example: measuresFilter__
```c++
//check if null or empty or in filter
                if (!measuresFilter //null
                    || measuresFilter->empty() //empty
                    || measuresFilter->find(codename) != measuresFilter->end() //codename matches
                    || measuresFilter->find(label) != measuresFilter->end()) // label matches
```
#### populateFromWelshStatsJSON
I ran into a couple of issues with this function, as the files sometimes contained slightly differing data. For example, the *tran0152.json* file used only the **SINGLE_MEASURE_CODE** and **SINGLE_MEASURE_NAME** whereas the rest contained a **MEASURE_CODE** and **MEASURE_NAME** from the passed cols enum map.

Additionlly, some of the values were stored as doubles whereas others were surrounded in quotation marks causing them to be read as strings. This meant that to read the data properly I had to enforce reading as double type to correctly use the data.

## Misc.
### cxxopts
In order to ensure I could run the program without providing a value for all flags, I encased each parsing function with an if statement checking to see if there were any argument values in the corresponding flag, instead passing the empty return type as if an *'all'* argument had been received, thereby creating default values for each of the flags.

__Example: areasFilter__
```c++
std::unordered_set<std::string> areasFilter;
      if (args.count("areas")) {
          areasFilter      = BethYw::parseAreasArg(args);
      } else {
          areasFilter.clear();
      }
```

I also made sure to catch all errors properly from the run function inside the BethYw class, and returned different codes for each overarching error type.

__Example__
```c++
catch (std::invalid_argument& iaError) {
      std::cerr << iaError.what() << std::endl;
      return 1;
  } catch (std::runtime_error& rtError) {
      std::cerr << "Error importing dataset:" << std::endl;
      std::cerr << rtError.what() << std::endl;
      return 2;
  }
```

### Padding By Finding Max Length In Value
I also implemented the padding and aligning right of the output table headers in a way which will allow modification of the number of decimal points desired in the values which are output in the future.

__Example__
```c++
//get max length of a measure value
//This ensures that if the number of decimal places for the values changes at a
//later date the column headers will still align
int maxLength = std::to_string(measure.getValues().begin()->second).size();
for (auto elem : measure.getValues()) {
int curLength = std::to_string(elem.second).size();
if (curLength > maxLength) {
maxLength = curLength;
}
}
```


### appending 0s to make the length of the doubles correct
I also discovered that many of the values are stored without the correct formatting by default. This meant that I had to add trailing 0s in some instances, and cut the value down in others. I achieved this by adding fixed and setprecision(6) markers to the output stream.

__Example__
```c++
 os << std::fixed << std::setprecision(6)<< value.second << ' ';
```

## Unimplemented
I did not manage to implement Task 8 - Implement extended argument filtering.
