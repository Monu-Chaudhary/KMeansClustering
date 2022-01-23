// Copyright [2020] chaudhm3@miamioh.edu

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "Kmeans.h"

/**
 * Reads data from the TSV file with the specified number of columns.
 * @param is The input file stream to read the data from.
 * @param col The specified number of columns to read.
 * @param  data The vector of points that is to be read from the file.
 */
void readTSV(std::istream& is, size_t col, PointList &data) {
    int i = col;
    bool flag = false;
    for (std::string line; std::getline(is, line);) {
        std::vector<double> pointVec;
        std::istringstream ifs(line);
        if (flag == true) {
            for (std::string value; ifs >> std::quoted(value);) {
                if (i > 0) {
                    pointVec.push_back(std::stod(value));
                    i--;
                }
            }
            std::valarray<double> val(pointVec.data(), pointVec.size());
            data.push_back(val);
        }
        i = col;
        flag = true;
    }
}

/**
 * Find the nearest centroid to the specific point.
 * @param k The centroids with which the point is compared.
 * @param p The specific point to be clustered.
 * @return The index position of the centroid in the list of centroids k.
 */
size_t nearestCentroidToPoint(PointList k, Point p) {
    double min = 100, d;
    size_t i = 0, ind = -1;
    for (Point j : k) {
        d = distance(j, p);
        if (min > d) {
            min = d;
            ind = i;
        }
        i++;
    }
    return ind;
}

/**
 * Find nearest centroid to each data point.
 * \param k The list of centroids.
 * \param  data The vector of points that is to be read from the file.  
 * \param centIdx The index of centroid for each data points.
 */
void centroidToDataPoints(PointList k, 
                        const PointList& data, IntVec& centIdx) {
    centIdx.erase(centIdx.begin(), centIdx.begin()+centIdx.size());
    for (Point point : data) {
        centIdx.push_back(nearestCentroidToPoint(k, point));
    }
}

/**
 * Update centroid coordinates based on assignment.
 * \param k The list of centroids.
 * \param  data The vector of points that is to be read from the file.  
 * \param centIdx The index of centroid for each data points.
 * \param colN The number of columns to be read.
 */
void updateCentroid(PointList& k, 
                const PointList& data, 
                IntVec centIdx,
                const size_t colN) {
    int i = 0;
    for (Point &c : k) {
        size_t count = 0;
        Point sum(0.0, colN);
        for (size_t j = 0; j < centIdx.size(); j++) {
            if (i == centIdx[j]) {
                count++;
                sum += data[j];
            }
        }
        for (size_t i = 0; i < colN; i++) {
            c[i] = sum[i] / count;
        }
        i++;
    }
}

/**
 * Operator overloding to perform == operation on PointList.
 * \param p1 First PointList argument to be compared.
 * \param p2 Second PointList argument to be compared
 * \return Returns a boolean value based on comparison.
 */
bool operator==(PointList& p1, PointList& p2) {
    for (size_t i = 0; i < p1.size(); i++) {
        for (size_t j = 0; j < p1[i].size(); j++) {
            if (p1[i][j] != p2[i][j]) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Perform the clustering operation over the data points.
 * \param k The list of centroids.
 * \param  data The vector of points that is to be clustered.  
 * \param colNum The number of columns .
 * \param colN The number of columns to be read.
 */
void Kmeans(PointList& k, 
                            const PointList& data, 
                            const size_t colNum,
                            IntVec& centIdx) {
    size_t reps = 100;
    size_t rep = 0;
    while (rep++ < reps) {
        // Centroid assignment for each data-point
        centroidToDataPoints(k, data, centIdx);

        // Update centroid coordinates based on assignment
        PointList prevCentroids = k;
        updateCentroid(k, data, centIdx, colNum);
        
        if (prevCentroids == k) {
            break;
        }
    }
}



int main(int argc, char *argv[]) {
    // Check if all the required arguments are passed.
    if (argc < 4) {
        std::cout << "Enter all the required arguments.\n";
        return 1;
    }

    // Check whether the file is good to open and read.
    std::ifstream is(argv[1]);
    if (!is.good()) {
        std::cout << "Error reading data from " << argv[1] << std::endl;
        return 2;
    }

    PointList data;
    const size_t colNum = std::stoi(argv[2]);
    const int k = std::stoi(argv[3]);
    IntVec idx(0, 0);

    // Function call to read the data from the TSV file.
    readTSV(is, colNum, data);

    // Initialize array of centroids
    PointList centroids = getInitCentroid(data, k);
    
    // Perform clustering if the number of centroids if greater than 0.
    if (centroids.size() != 0) {
         Kmeans(centroids, data, colNum, idx);
    }

    // Write output to the output stream.
    writeResults(data, centroids, idx, std::cout);
    return 0;
}
