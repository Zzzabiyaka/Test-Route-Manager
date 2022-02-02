#include "gtest/gtest.h"

#include "RouteManager.h"
#include <fstream>

TEST(BasicFunctional, AddingRoutes) {
  RouteManager manager;

  ASSERT_EQ(manager.AddRoute("A", "B", 1), RouteManager::OK);
  ASSERT_EQ(manager.AddRoute("B", "B", 1), RouteManager::ABORTED);
  ASSERT_EQ(manager.AddRoute("A", "B", 11), RouteManager::ABORTED);
  ASSERT_EQ(manager.AddRoute("A", "B", 1), RouteManager::OK);
  ASSERT_EQ(manager.AddRoute("A", "A", 1), RouteManager::ABORTED);
  ASSERT_EQ(manager.AddRoute("A", "C", -1), RouteManager::ABORTED);
}

TEST(BasicFunctional, FindTheLengthOfRoute) {
  RouteManager manager;
  manager.AddRoute("A", "B", 1);
  manager.AddRoute("B", "C", 2);

  ASSERT_EQ(manager.FindTheLengthOfRoute("A", "B", "C").value(), 3);
  ASSERT_FALSE(manager.FindTheLengthOfRoute("A", "B", "B").has_value());
  ASSERT_FALSE(manager.FindTheLengthOfRoute("A", "B", "A", "B").has_value());
}

TEST(BasicFunctional, ShortestPath) {
  RouteManager manager;
  manager.AddRoute("A", "B", 1);
  manager.AddRoute("A", "C", 4);
  manager.AddRoute("B","C", 2);

  ASSERT_EQ(manager.FindTheLengthOfShortestPath("A", "B"), 1);
  ASSERT_EQ(manager.FindTheLengthOfShortestPath("B", "C"), 2);
  ASSERT_EQ(manager.FindTheLengthOfShortestPath("A", "C"), 3);
  ASSERT_FALSE(manager.FindTheLengthOfShortestPath("B", "A").has_value());
}

TEST(BasicFunctional, RoutesLimitedByStops) {
  {
    RouteManager manager;
    manager.AddRoute("A", "B", 2);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A","B",1,
                                                        RouteManager::StrictPolicy::STRICT,
                                                        RouteManager::Limitation::STOPS), 1);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A",
                                                        "B",
                                                        1,
                                                        RouteManager::StrictPolicy::NOT_STRICT,
                                                        RouteManager::Limitation::STOPS), 1);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A",
                                                        "B",
                                                        2,
                                                        RouteManager::StrictPolicy::NOT_STRICT,
                                                        RouteManager::Limitation::STOPS), 1);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A",
                                                        "B",
                                                        2,
                                                        RouteManager::StrictPolicy::STRICT,
                                                        RouteManager::Limitation::STOPS), 0);
  }

  {
    RouteManager manager;
    manager.AddRoute("A", "B", 2);
    manager.AddRoute("B", "C", 1);

    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A",
                                                        "C",
                                                        1,
                                                        RouteManager::StrictPolicy::STRICT,
                                                        RouteManager::Limitation::STOPS), 0);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A",
                                                        "C",
                                                        2,
                                                        RouteManager::StrictPolicy::NOT_STRICT,
                                                        RouteManager::Limitation::STOPS), 1);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A",
                                                        "C",
                                                        2,
                                                        RouteManager::StrictPolicy::STRICT,
                                                        RouteManager::Limitation::STOPS), 1);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A",
                                                        "C",
                                                        3,
                                                        RouteManager::StrictPolicy::STRICT,
                                                        RouteManager::Limitation::STOPS), 0);
  }

  {
    RouteManager manager;
    manager.AddRoute("A", "B", 1);
    manager.AddRoute("B", "A", 1);
    manager.AddRoute("B", "C", 1);
    manager.AddRoute("C", "A", 1);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A",
                                                        "C",
                                                        100,
                                                        RouteManager::StrictPolicy::NOT_STRICT,
                                                        RouteManager::Limitation::STOPS), 1);
  }
}

TEST(BasicFunctional, RoutesLimitedByTime) {
  {
    RouteManager manager;
    manager.AddRoute("A", "B", 2);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A", "B", 1, RouteManager::StrictPolicy::STRICT, RouteManager::Limitation::DAYS), 0);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A", "B", 1, RouteManager::StrictPolicy::NOT_STRICT, RouteManager::Limitation::DAYS), 0);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A", "B", 2, RouteManager::StrictPolicy::NOT_STRICT, RouteManager::Limitation::DAYS), 1);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A",
                                                        "B",
                                                        2,
                                                        RouteManager::StrictPolicy::STRICT, RouteManager::Limitation::DAYS), 1);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A", "B", 3, RouteManager::StrictPolicy::NOT_STRICT, RouteManager::Limitation::DAYS), 1);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A", "B", 3, RouteManager::StrictPolicy::STRICT, RouteManager::Limitation::DAYS), 0);
  }

  {
    RouteManager manager;
    manager.AddRoute("A", "B", 2);
    manager.AddRoute("B", "C", 1);

    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A", "C", 1, RouteManager::StrictPolicy::STRICT, RouteManager::Limitation::DAYS), 0);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A", "C", 3, RouteManager::StrictPolicy::NOT_STRICT, RouteManager::Limitation::DAYS), 1);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A", "C", 3, RouteManager::StrictPolicy::STRICT, RouteManager::Limitation::DAYS), 1);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A", "C", 4, RouteManager::StrictPolicy::STRICT, RouteManager::Limitation::DAYS), 0);
  }

  {
    RouteManager manager;
    manager.AddRoute("A", "B", 1);
    manager.AddRoute("B", "A", 1);
    manager.AddRoute("B", "C", 1);
    manager.AddRoute("C", "A", 1);
    ASSERT_EQ(manager.FindNumRoutesLimitedByTimeOrStops("A", "C", 100, RouteManager::StrictPolicy::NOT_STRICT, RouteManager::Limitation::DAYS), 1);
  }
}

TEST(MainTest, YourData) {
  std::ofstream fout("results.txt");
  RouteManager manager;
  manager.AddRoute("Buenos Aires", "New York", 6);
  manager.AddRoute("Buenos Aires", "Casablanca", 5);
  manager.AddRoute("Buenos Aires", "Cape Town", 4);
  manager.AddRoute("New York", "Liverpool", 4);
  manager.AddRoute("Liverpool", "Casablanca", 3);
  manager.AddRoute("Liverpool", "Cape Town", 6);
  manager.AddRoute("Casablanca", "Liverpool", 3);
  manager.AddRoute("Casablanca", "Cape Town", 6);
  manager.AddRoute("Cape Town", "New York", 8);

  {
    auto res =
        manager.FindTheLengthOfRoute("Buenos Aires", "New York", "Liverpool");
    if (res.has_value()) {
      fout << "Time for Buenos Aires -> New York -> Liverpool is "
           << res.value() << '\n';
    } else {
      fout << "Sorry, but Buenos Aires -> New York -> Liverpool doesn't exist\n";
    }
  }

  {
    auto res =
        manager.FindTheLengthOfRoute("Buenos Aires", "Casablanca", "Liverpool");
    if (res.has_value()) {
      fout << "Time for Buenos Aires -> Casablanca -> Liverpool is "
           << res.value() << '\n';
    } else {
      fout << "Sorry, but Buenos Aires -> Casablanca -> Liverpool doesn't exist\n";
    }
  }

  {
    auto res =
        manager.FindTheLengthOfRoute("Buenos Aires", "Cape Town", "New York", "Liverpool", "Casablanca");
    if (res.has_value()) {
      fout
          << "Time for Buenos Aires -> Cape Town -> New York -> Liverpool -> Casablanca is "
          << res.value() << '\n';
    } else {
      fout << "Sorry, but Buenos Aires -> Cape Town -> New York -> Liverpool -> Casablanca doesn't exist\n";
    }
  }

  {
    auto res =
        manager.FindTheLengthOfRoute("Buenos Aires", "Cape Town", "Casablanca");
    if (res.has_value()) {
      fout << "Time for Buenos Aires -> Cape Town -> Casablanca is "
           << res.value() << '\n';
    } else {
      fout << "Sorry, but Buenos Aires -> Cape Town -> Casablanca doesn't exist\n";
    }
  }

  {
    auto res = manager.FindTheLengthOfShortestPath("Buenos Aires", "Liverpool");
    if (res.has_value()) {
      fout << "The shortest path between Buenos Aires and Liverpool has the length of " << res.value() << '\n';
    } else {
      fout << "Sorry but path between Buenos Aires and Liverpool doesn't exist";
    }
  }

  {
    auto res = manager.FindTheLengthOfShortestPath("New York", "New York");
    if (res.has_value()) {
      fout << "The shortest path between New York and New York has the length of " << res.value() << '\n';
    } else {
      fout << "Sorry but path between New York and New York doesn't exist";
    }
  }

  {
    auto res = manager.FindNumRoutesLimitedByTimeOrStops("Liverpool",
                                                         "Liverpool",
                                                         3,
                                                         RouteManager::StrictPolicy::NOT_STRICT, RouteManager::Limitation::STOPS);
    fout << "Num of routes from Liverpool to Liverpool with maximum number of 3 stops is " << res << '\n';
  }
  {
    auto res = manager.FindNumRoutesLimitedByTimeOrStops("Buenos Aires",
                                                         "Liverpool",
                                                         4,
                                                         RouteManager::StrictPolicy::STRICT, RouteManager::Limitation::STOPS);
    fout << "Num of routes from Liverpool to Liverpool with exactly 4 stops is " << res << '\n';
  }
  {
    auto res = manager.FindNumRoutesLimitedByTimeOrStops("Liverpool", "Liverpool", 25, RouteManager::StrictPolicy::NOT_STRICT, RouteManager::Limitation::DAYS);
    fout << "Num of routes from Liverpool to Liverpool with less or equal than 25 days is " << res << '\n';
  }
}