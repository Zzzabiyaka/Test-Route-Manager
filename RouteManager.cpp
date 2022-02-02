#include "RouteManager.h"
#include <iostream>

RouteManager::ExitCode RouteManager::AddRoute(const std::string& departure,
                                              const std::string& destination,
                                              int duration) noexcept {
  // Should check whether we see city firstly and increment Id if we are
  for (const auto& city: {departure, destination}) {
    if (!id_by_city_.count(city)) {
      id_by_city_[city] = id_++;
      city_by_id_.push_back(city);
    }
  }

  // Check if Such a route exist with a different duration
  // (говоря по-русски, проверяю есть ли кратное ребро, если есть, в рамках задачи считаю, что добавление некорректное)
  if (auto route =
        std::make_pair(id_by_city_[departure], id_by_city_[destination]);
      routes_.count(route) && routes_[route] != duration) {
    return ABORTED;
  }

  //Check if departure isn't the destination. If it is I think Route isn't good, so ABORTED is returned.
  if (departure == destination) {
    return ABORTED;
  }

  // If duration less than 0 then aborted.
  if (duration < 0) {
    return ABORTED;
  }


  // Adding to the internal graph our edge
  graph_.resize(id_);
  graph_[id_by_city_[departure]].push_back({id_by_city_[destination],
                                            duration});
  routes_[{id_by_city_[departure], id_by_city_[destination]}] = duration;

  return OK;
}

std::optional<int> RouteManager::FindTheLengthOfShortestPath(const std::string& departure,
                                                             const std::string& arrival) {
  if (dist_.empty()) {
    InternalFloyd();
  }

  return (dist_[id_by_city_[departure]][id_by_city_[arrival]] == kInfty
          ? std::optional<int>() :
          dist_[id_by_city_[departure]][id_by_city_[arrival]]);
}
void RouteManager::InternalFloyd() {
  dist_.resize(id_);
  for (auto& row: dist_) {
    row.resize(id_);
    for (auto& j: row) {
      j = kInfty;
    }
  }

  for (int i = 0; i < id_; ++i) {
    dist_[i][i] = 0;
  }

  for (int i = 0; i < graph_.size(); ++i) {
    for (int j = 0; j < graph_[i].size(); ++j) {
      dist_[i][graph_[i][j].first] = graph_[i][j].second;
    }
  }

  for (int k = 0; k < id_; ++k) {
    for (int i = 0; i < id_; ++i) {
      for (int j = 0; j < id_; ++j) {
        if (dist_[i][k] != kInfty && dist_[k][j] != kInfty) {
          dist_[i][j] = std::min(dist_[i][j], dist_[i][k] + dist_[k][j]);
        }
      }
    }
  }
}
int RouteManager::FindNumRoutesLimitedByTimeOrStops(const std::string& departure,
                                                    const std::string& arrival,
                                                    int stops_num,
                                                    StrictPolicy strict_policy,
                                                    Limitation limitation) {
  int result = 0;
  std::vector<bool> used(id_, false);
  if (departure == arrival) {
    if (strict_policy == RouteManager::NOT_STRICT ||
    (strict_policy == RouteManager::STRICT && stops_num == 0)) {
      result++;
    }
    int dep_id = id_by_city_[departure];
    for(auto edge : graph_[dep_id]) {
      if (limitation == Limitation::STOPS) {
        InternalDfs(edge.first,
                    id_by_city_[arrival],
                    &used,
                    &result,
                    stops_num - 1,
                    limitation,
                    strict_policy);
      } else {
        InternalDfs(edge.first,
                    id_by_city_[arrival],
                    &used,
                    &result,
                    stops_num - edge.second,
                    limitation,
                    strict_policy);
      }
    }
  } else {
    InternalDfs(id_by_city_[departure],
                id_by_city_[arrival],
                &used,
                &result,
                stops_num,
                limitation,
                strict_policy);
  }

  return result;
}
void RouteManager::InternalDfs(int vertex,
                               int target_vertex,
                               std::vector<bool>* used,
                               int* result,
                               int target_num,
                               Limitation limitation,
                               StrictPolicy strict_policy) {
  if (vertex == target_vertex || target_num < 0) {
    if ((target_num >= 0 && strict_policy == StrictPolicy::NOT_STRICT) ||
        (target_num == 0 && strict_policy == StrictPolicy::STRICT)) {
      *result = *result + 1;
    }
    return;
  }

  (*used)[vertex] = true;

  for(auto& edge : graph_[vertex]) {
    if (edge.first == target_vertex || !(*used)[edge.first]) {
      if (limitation == Limitation::STOPS) {
        InternalDfs(edge.first,
                    target_vertex,
                    used,
                    result,
                    target_num - 1,
                    limitation,
                    strict_policy);
      } else {
        InternalDfs(edge.first,
                    target_vertex,
                    used,
                    result,
                    target_num - edge.second,
                    limitation,
                    strict_policy);
      }
    }
  }

  (*used)[vertex] = false;
}
