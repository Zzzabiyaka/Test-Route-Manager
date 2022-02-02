#ifndef ROUTEMANAGER_H
#define ROUTEMANAGER_H

#include <initializer_list>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>


class RouteManager {
 public:
  enum ExitCode {
    OK,
    ABORTED
  };
  enum StrictPolicy {
    STRICT,
    NOT_STRICT
  };
  enum Limitation {
    STOPS,
    DAYS
  };

  ExitCode AddRoute(const std::string& departure,
                    const std::string& destination,
                    int duration) noexcept;

  template<typename... Strings>
  std::optional<int> FindTheLengthOfRoute(Strings... cities) {
    std::vector<std::string> cit{cities...};
    if (cit.empty()) {
      return {};
    }

    if (cit.size() == 1) {
      return 0;
    }

    int result = 0;
    for (int i = 0; i + 1 < cit.size(); ++i) {
      std::pair<int, int>
          route = {id_by_city_.at(cit[i]), id_by_city_.at(cit[i + 1])};
      if (!routes_.count(route)) {
        return {};
      }
      result += routes_[route];
    }

    return result;
  }

  std::optional<int> FindTheLengthOfShortestPath(const std::string& departure,
                                  const std::string& arrival);

  int FindNumRoutesLimitedByTimeOrStops(const std::string& departure,
                                 const std::string& arrival,
                                 int time, StrictPolicy strict_policy, Limitation limitation);
 private:
  void InternalDfs(int vertex,
                   int target_vertex,
                   std::vector<bool>* used,
                   int* result,
                   int target_num,
                   Limitation limitation,
                   StrictPolicy strict_policy);
  void InternalFloyd();
  std::vector<std::vector<std::pair<int, int>>> graph_;
  int id_ = 0;
  const int kInfty = std::numeric_limits<int>::max();
  std::vector<std::string> city_by_id_;
  std::vector<std::vector<int>> dist_;
  std::unordered_map<std::string, int> id_by_city_;
  std::map<std::pair<int, int>, int> routes_;

};

#endif //ROUTEMANAGER_H
