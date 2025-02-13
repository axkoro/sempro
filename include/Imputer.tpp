#include <cmath>

#include "Imputer.hpp"  // because IntelliSense is stupid

template <typename T>
double Imputer<T>::compute_global_average(int feature) {
    double sum = 0.0;
    int count = 0;
    for (int node = 0; node < graph.get_num_nodes(); node++) {
        if (!graph.is_missing(node, feature)) {
            sum += graph.get_feature(node, feature);
            count++;
        }
    }

    return (count > 0) ? (sum / count) : 0;
}

template <typename T>
T round_value(double value) {
    if constexpr (std::is_same_v<T, bool>) {
        return (value >= 0.5);
    } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
        return static_cast<T>(std::round(value));
    } else {
        return static_cast<T>(value);
    }
}