# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## master
### Changed
- Mark function definitions from `DR_PARAM_DEFINE_YAML_*` as inline to allow them to be used in headers.

## 2.0.0 - 2021-01-05
### Changed
- Export boost and dr_util as transitive dependency to facilitate static linking.
### Removed
- Remove support for the ROS parameter server.

## 1.3.0 - 2020-11-09
### Added
- Add `mergeYamlNodes` to merge YAML nodes.

## 1.2.4 - 2020-09-30
### Changed
- Fix namespace problems for encoding of std::map.

## 1.2.3 - 2020-07-06
### Changed
- Add missing `#include <sstream>` in `yaml.cpp`

## 1.2.2 - 2020-06-02
### Added
- Add LICENSE file with Apache v2.0 license.

### Changed
- Link with --as-needed.

## 1.2.1 - 2020-05-18
### Changed
- Fix missing include for std::runtime_error.

## 1.2.0 - 2020-03-23
### Added
- Possibility to parse YAML nodes in YAML::Node.

### Changed
- Fix deprecation warnings during compilation and in tests.
