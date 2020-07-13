[![<SilverLinings89>](https://circleci.com/gh/SilverLinings89/HallmarksSearchTool.svg?style=svg)](https://app.circleci.com/pipelines/github/SilverLinings89)

# The Hallmarks Search Tool

This tool is supposed to crawl a given dataset and automatically build a codeword-rating for predefined hallmarks. All information required is:

- a large set of publications and their abstract contents,
- a set of hallmarks and a short description text

With these two ingredients, this scheme can be used to build ratings for publications on all of the given hallmarks automatically, as well as provide text classification and nearest-neighbor type search in the hallmark space.

## Usage

Usage consists of two steps:

1. Training on a given dataset. This takes several hours but only has to be done once.
2. Using the trained system to perform classification on abstracts of scientific publications. This can be done in milliseconds and can even be done in a browser online.

### Usage of the code for step 1

The crawler is implemented in C++ (language standard 17), and uses a cmake build system.
It requires Boost to be available as a dev-dependency (i.e. in ubuntu `sudo apt-get install libboost1.65-all-dev` should suffice).
It also uses openmp for loop parallelization.

The build can be started by calling `cmake .` in the root directory of the repository. After successful completion, call make. A binary will be created that can simply be executed to run the system.

### Step 2

I will eventually create an online interface to access the system via the web and implement a search functionality for it.

## Tests

Gtest has been added and some unit-tests are present. They are automated in a CI-Pipeline in the CircleCi build.
Also: Gtest is a git-submodule in the third_party subfolder.

## Data

I have a prepared datafile that has the following structure

#ID# \t #"Title of the publication"# \t "Abstract text" \n

where #ID# is a unique identifier for the publication, the title is the title of the scientific publication and the abstract text is the third field available. \t refers to tab-separation and \n is the end of the line.
I created my 340MB input file by crawling the publically available data at Springer.
