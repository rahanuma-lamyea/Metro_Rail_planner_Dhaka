#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <fstream>
#include <sstream>
#include <cmath>
#include <limits>
#include <stack>
#include<bits/stdc++.h>
using namespace std;

// Template class for generic Pair
template <typename T>
class GenericPair {
protected:
    T first;
    T second;
public:
    GenericPair() : first(T()), second(T()) {}
    GenericPair(T f, T s) : first(f), second(s) {}

    T getFirst() const { return first; }
    T getSecond() const { return second; }
    void setFirst(T f) { first = f; }
    void setSecond(T s) { second = s; }

    bool operator==(const GenericPair& other) const {
        return first == other.first && second == other.second;
    }

    friend ostream& operator<<(ostream& os, const GenericPair& p) {
        os << "(" << p.first << ", " << p.second << ")";
        return os;
    }
};

// Base class for Station (Virtual base class)
class Station {
protected:
    string name;
    string lineCode;
    static int totalStations;

public:
    Station() : name(""), lineCode("") {
        totalStations++;
    }

    Station(string n, string code) : name(n), lineCode(code) {
        totalStations++;
    }

    virtual ~Station() {
        totalStations--;
    }

    virtual void display() const = 0;
    virtual string getFullName() const = 0;

    string getName() const { return name; }
    string getLineCode() const { return lineCode; }
    static int getTotalStations() { return totalStations; }

    bool operator==(const Station& other) const {
        return name == other.name && lineCode == other.lineCode;
    }
};

int Station::totalStations = 0;

// Derived class for Metro Station
class MetroStation : public virtual Station {
private:
    unordered_map<string, int> neighbours;

public:
    MetroStation() : Station() {}
    MetroStation(string n, string code) : Station(n, code) {}

    MetroStation(const MetroStation& other) : Station(other.name, other.lineCode) {
        neighbours = other.neighbours;
    }

    void display() const override {
        cout << name << " [Line: " << lineCode << "]";
    }

    string getFullName() const override {
        return name + "~" + lineCode;
    }

    void addNeighbour(string station, int distance) {
        neighbours[station] = distance;
    }

    void removeNeighbour(string station) {
        neighbours.erase(station);
    }

    unordered_map<string, int>& getNeighbours() {
        return neighbours;
    }

    const unordered_map<string, int>& getNeighbours() const {
        return neighbours;
    }

    bool hasNeighbour(string station) const {
        return neighbours.find(station) != neighbours.end();
    }

    int getDistance(string station) const {
        auto it = neighbours.find(station);
        return (it != neighbours.end()) ? it->second : -1;
    }

    MetroStation& operator=(const MetroStation& other) {
        if (this != &other) {
            name = other.name;
            lineCode = other.lineCode;
            neighbours = other.neighbours;
        }
        return *this;
    }
};

// Path Information class
class PathInfo : public GenericPair<string> {
private:
    int distance;
    int time;

public:
    PathInfo() : GenericPair<string>(), distance(0), time(0) {}
    PathInfo(string src, string dest, int dist, int t)
        : GenericPair<string>(src, dest), distance(dist), time(t) {}

    int getDistance() const { return distance; }
    int getTime() const { return time; }

    PathInfo operator+(const PathInfo& other) const {
        PathInfo result;
        result.distance = this->distance + other.distance;
        result.time = this->time + other.time;
        return result;
    }

    bool operator<(const PathInfo& other) const {
        return distance < other.distance;
    }
};

// File Handler class
class FileHandler {
private:
    string filename;

public:
    FileHandler(string fname) : filename(fname) {}

    void saveStations(const unordered_map<string, MetroStation>& stations) {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cerr << "Error opening file for writing!" << endl;
            return;
        }

        outFile << "STATIONS:" << endl;
        for (const auto& pair : stations) {
            outFile << pair.first << endl;
        }

        outFile << "EDGES:" << endl;
        for (const auto& pair : stations) {
            const MetroStation& station = pair.second;
            for (const auto& neighbour : station.getNeighbours()) {
                outFile << pair.first << "," << neighbour.first << "," << neighbour.second << endl;
            }
        }

        outFile.close();
        cout << "\nData saved to " << filename << " successfully!" << endl;
    }

    void loadStations(unordered_map<string, MetroStation>& stations) {
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            cerr << "File not found. Using default metro map." << endl;
            return;
        }

        string line;
        bool readingStations = false;
        bool readingEdges = false;

        while (getline(inFile, line)) {
            if (line == "STATIONS:") {
                readingStations = true;
                readingEdges = false;
                continue;
            }
            if (line == "EDGES:") {
                readingStations = false;
                readingEdges = true;
                continue;
            }

            if (readingStations && !line.empty()) {
                size_t pos = line.find('~');
                if (pos != string::npos) {
                    string name = line.substr(0, pos);
                    string code = line.substr(pos + 1);
                    stations[line] = MetroStation(name, code);
                }
            }
            else if (readingEdges && !line.empty()) {
                stringstream ss(line);
                string station1, station2, distStr;
                if (getline(ss, station1, ',') && getline(ss, station2, ',') && getline(ss, distStr)) {
                    try {
                        int distance = stoi(distStr);
                        if (stations.find(station1) != stations.end()) {
                            stations[station1].addNeighbour(station2, distance);
                        }
                    } catch (const exception& e) {
                        cerr << "Error parsing distance: " << distStr << endl;
                    }
                }
            }
        }

        inFile.close();
        cout << "\nData loaded from " << filename << " successfully!" << endl;
    }
};

// Dijkstra Pair class
class DijkstraPair {
public:
    string vname;
    string psf;
    int cost;

    DijkstraPair() : vname(""), psf(""), cost(INT_MAX) {}
    DijkstraPair(string v, string p, int c) : vname(v), psf(p), cost(c) {}

    bool operator<(const DijkstraPair& other) const {
        return cost > other.cost;
    }

    bool operator>(const DijkstraPair& other) const {
        return cost < other.cost;
    }
};

// Search Pair class
class SearchPair {
public:
    string vname;
    string psf;
    int minDis;
    int minTime;

    SearchPair() : vname(""), psf(""), minDis(0), minTime(0) {}
    SearchPair(string v, string p, int d, int t)
        : vname(v), psf(p), minDis(d), minTime(t) {}
};

// Base Graph class
class Graph {
protected:
    static int graphCount;

public:
    Graph() { graphCount++; }
    virtual ~Graph() { graphCount--; }

    virtual void addVertex(string vname) = 0;
    virtual void addEdge(string v1, string v2, int weight) = 0;
    virtual void display() const = 0;
    virtual bool hasPath(string src, string dest) = 0;

    static int getGraphCount() { return graphCount; }
};

int Graph::graphCount = 0;

// Metro Graph class
class MetroGraph : public Graph {
private:
    unordered_map<string, MetroStation> stations;
    FileHandler* fileHandler;

    vector<string> getInterchanges(string str) {
        vector<string> arr;
        vector<string> tokens;

        stringstream ss(str);
        string token;
        while (ss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) return arr;

        arr.push_back(tokens[0]);
        int interchangeCount = 0;

        for (size_t i = 1; i < tokens.size() - 1; i++) {
            size_t index = tokens[i].find('~');
            if (index == string::npos) continue;

            string s = tokens[i].substr(index + 1);

            if (s.length() == 2) {
                string prev = tokens[i - 1].substr(tokens[i - 1].find('~') + 1);
                string next = tokens[i + 1].substr(tokens[i + 1].find('~') + 1);

                if (prev == next) {
                    arr.push_back(tokens[i]);
                } else {
                    arr.push_back(tokens[i] + " ==> " + tokens[i + 1]);
                    i++;
                    interchangeCount++;
                }
            } else {
                arr.push_back(tokens[i]);
            }
        }

        if (!tokens.empty()) {
            arr.push_back(tokens[tokens.size() - 1]);
        }
        arr.push_back(to_string(interchangeCount));

        return arr;
    }

public:
    MetroGraph() : Graph() {
        fileHandler = new FileHandler("metro_data.txt");
    }

    MetroGraph(string filename) : Graph() {
        fileHandler = new FileHandler(filename);
    }

    MetroGraph(const MetroGraph& other) : Graph() {
        stations = other.stations;
        fileHandler = new FileHandler("metro_data.txt");
    }

    ~MetroGraph() {
        delete fileHandler;
    }

    void addVertex(string vname) override {
        size_t pos = vname.find('~');
        if (pos == string::npos) {
            cerr << "Invalid station format: " << vname << endl;
            return;
        }
        string name = vname.substr(0, pos);
        string code = vname.substr(pos + 1);
        stations[vname] = MetroStation(name, code);
    }

    void addEdge(string v1, string v2, int weight) override {
        if (stations.find(v1) == stations.end() || stations.find(v2) == stations.end()) {
            cerr << "One or both stations not found: " << v1 << ", " << v2 << endl;
            return;
        }
        if (stations[v1].hasNeighbour(v2)) {
            return;
        }

        stations[v1].addNeighbour(v2, weight);
        stations[v2].addNeighbour(v1, weight);
    }

    void removeVertex(string vname) {
        if (stations.find(vname) == stations.end()) return;

        MetroStation& vtx = stations[vname];
        vector<string> neighbours;

        for (const auto& pair : vtx.getNeighbours()) {
            neighbours.push_back(pair.first);
        }

        for (const string& nbr : neighbours) {
            stations[nbr].removeNeighbour(vname);
        }

        stations.erase(vname);
    }

    void removeEdge(string v1, string v2) {
        if (stations.find(v1) == stations.end() || stations.find(v2) == stations.end()) {
            return;
        }

        stations[v1].removeNeighbour(v2);
        stations[v2].removeNeighbour(v1);
    }

    bool containsVertex(string vname) const {
        return stations.find(vname) != stations.end();
    }

    bool containsEdge(string v1, string v2) const {
        if (stations.find(v1) == stations.end() || stations.find(v2) == stations.end()) {
            return false;
        }
        return stations.at(v1).hasNeighbour(v2);
    }

    int numVertices() const {
        return stations.size();
    }

    int numEdges() const {
        int count = 0;
        for (const auto& pair : stations) {
            count += pair.second.getNeighbours().size();
        }
        return count / 2;
    }

    void display() const override {
        cout << "\n\t Dhaka Metro Map" << endl;
        cout << "\t------------------" << endl;
        cout << "----------------------------------------------------" << endl;

        for (const auto& pair : stations) {
            cout << "\n" << pair.first << " =>" << endl;
            for (const auto& nbr : pair.second.getNeighbours()) {
                cout << "\t" << nbr.first << "\t\t" << nbr.second << " KM" << endl;
            }
        }

        cout << "\n\t------------------" << endl;
        cout << "---------------------------------------------------" << endl;
    }

    void displayStations() const {
        cout << "\n***********************************************" << endl;
        cout << "           METRO STATIONS LIST" << endl;
        cout << "***********************************************\n" << endl;

        int i = 1;
        for (const auto& pair : stations) {
            cout << i << ". " << pair.first << endl;
            i++;
        }

        cout << "\n***********************************************" << endl;
    }

    bool hasPath(string src, string dest) override {
        if (!containsVertex(src) || !containsVertex(dest)) {
            return false;
        }
        unordered_map<string, bool> visited;
        return hasPathHelper(src, dest, visited);
    }

private:
    bool hasPathHelper(string src, string dest, unordered_map<string, bool>& visited) {
        if (src == dest) {
            return true;
        }

        visited[src] = true;

        for (const auto& pair : stations.at(src).getNeighbours()) {
            string nbr = pair.first;
            if (visited.find(nbr) == visited.end()) {
                if (hasPathHelper(nbr, dest, visited)) {
                    return true;
                }
            }
        }

        return false;
    }

public:
    int dijkstra(string src, string dest, bool timeWise) {
        if (!containsVertex(src) || !containsVertex(dest)) {
            return -1;
        }

        unordered_map<string, DijkstraPair> map;
        priority_queue<DijkstraPair> pq;

        for (const auto& pair : stations) {
            string key = pair.first;
            DijkstraPair np;
            np.vname = key;
            np.cost = INT_MAX;

            if (key == src) {
                np.cost = 0;
                np.psf = key;
            }

            pq.push(np);
            map[key] = np;
        }

        while (!pq.empty()) {
            DijkstraPair rp = pq.top();
            pq.pop();

            if (rp.vname == dest) {
                return rp.cost;
            }

            if (map.find(rp.vname) == map.end()) {
                continue;
            }

            map.erase(rp.vname);

            const MetroStation& station = stations.at(rp.vname);
            for (const auto& pair : station.getNeighbours()) {
                string nbr = pair.first;
                if (map.find(nbr) != map.end()) {
                    int oldCost = map[nbr].cost;
                    int newCost;

                    if (timeWise) {
                        newCost = rp.cost + 120 + 40 * pair.second;
                    } else {
                        newCost = rp.cost + pair.second;
                    }

                    if (newCost < oldCost) {
                        DijkstraPair gp = map[nbr];
                        gp.psf = rp.psf + " " + nbr;
                        gp.cost = newCost;
                        map[nbr] = gp;

                        // Create new object for priority queue
                        DijkstraPair newPair(gp.vname, gp.psf, gp.cost);
                        pq.push(newPair);
                    }
                }
            }
        }

        return -1;
    }

    string getMinimumDistance(string src, string dest) {
        if (!containsVertex(src) || !containsVertex(dest)) {
            return "";
        }

        int minDist = INT_MAX;
        string ans = "";
        unordered_map<string, bool> processed;
        stack<SearchPair> st;

        SearchPair sp;
        sp.vname = src;
        sp.psf = src + "  ";
        sp.minDis = 0;
        sp.minTime = 0;

        st.push(sp);

        while (!st.empty()) {
            SearchPair rp = st.top();
            st.pop();

            if (processed[rp.vname]) {
                continue;
            }

            processed[rp.vname] = true;

            if (rp.vname == dest) {
                if (rp.minDis < minDist) {
                    ans = rp.psf;
                    minDist = rp.minDis;
                }
                continue;
            }

            const MetroStation& station = stations.at(rp.vname);
            for (const auto& pair : station.getNeighbours()) {
                string nbr = pair.first;
                if (!processed[nbr]) {
                    SearchPair np;
                    np.vname = nbr;
                    np.psf = rp.psf + nbr + "  ";
                    np.minDis = rp.minDis + pair.second;
                    st.push(np);
                }
            }
        }

        if (minDist != INT_MAX) {
            ans += to_string(minDist);
        }
        return ans;
    }

    string getMinimumTime(string src, string dest) {
        if (!containsVertex(src) || !containsVertex(dest)) {
            return "";
        }

        int minTime = INT_MAX;
        string ans = "";
        unordered_map<string, bool> processed;
        stack<SearchPair> st;

        SearchPair sp;
        sp.vname = src;
        sp.psf = src + "  ";
        sp.minDis = 0;
        sp.minTime = 0;

        st.push(sp);

        while (!st.empty()) {
            SearchPair rp = st.top();
            st.pop();

            if (processed[rp.vname]) {
                continue;
            }

            processed[rp.vname] = true;

            if (rp.vname == dest) {
                if (rp.minTime < minTime) {
                    ans = rp.psf;
                    minTime = rp.minTime;
                }
                continue;
            }

            const MetroStation& station = stations.at(rp.vname);
            for (const auto& pair : station.getNeighbours()) {
                string nbr = pair.first;
                if (!processed[nbr]) {
                    SearchPair np;
                    np.vname = nbr;
                    np.psf = rp.psf + nbr + "  ";
                    np.minTime = rp.minTime + 120 + 40 * pair.second;
                    st.push(np);
                }
            }
        }

        if (minTime != INT_MAX) {
            double minutes = ceil((double)minTime / 60);
            ans += to_string((int)minutes);
        }
        return ans;
    }

    vector<int> getMinimumFare(string src, string dest, bool distanceWise) {
        int stationCount = 0;
        int fare = 0;

        if (distanceWise) {
            vector<string> path = getInterchanges(getMinimumDistance(src, dest));
            stationCount = path.empty() ? 0 : path.size() - 2;
        } else {
            vector<string> path = getInterchanges(getMinimumTime(src, dest));
            stationCount = path.empty() ? 0 : path.size() - 2;
        }

        if (stationCount > 0 && stationCount <= 3)
            fare = 20;
        else if (stationCount > 3 && stationCount <= 5)
            fare = 30;
        else if (stationCount > 5 && stationCount <= 9)
            fare = 40;
        else if (stationCount > 9 && stationCount <= 15)
            fare = 60;
        else if (stationCount > 15)
            fare = 100;

        return {stationCount, fare};
    }

    void saveToFile() {
        fileHandler->saveStations(stations);
    }

    void loadFromFile() {
        fileHandler->loadStations(stations);
    }

    MetroGraph& operator+=(const string& stationName) {
        addVertex(stationName);
        return *this;
    }

    friend ostream& operator<<(ostream& os, const MetroGraph& graph) {
        os << "Metro Graph with " << graph.numVertices() << " stations and "
           << graph.numEdges() << " connections.";
        return os;
    }

    static void createDefaultMetroMap(MetroGraph& g) {
        // Add all 16 Dhaka Metro Rail Line-6 stations
        g.addVertex("Uttara_North~M6");
        g.addVertex("Uttara_Center~M6");
        g.addVertex("Uttara_South~M6");
        g.addVertex("Pallabi~M6");
        g.addVertex("Mirpur_11~M6");
        g.addVertex("Mirpur_10~M6");
        g.addVertex("Kazipara~M6");
        g.addVertex("Shewrapara~M6");
        g.addVertex("Agargaon~M6");
        g.addVertex("Bijoy_Sarani~M6");
        g.addVertex("Farmgate~M6");
        g.addVertex("Karwan_Bazar~M6");
        g.addVertex("Shahbag~M6");
        g.addVertex("Dhaka_University~M6");
        g.addVertex("Bangladesh_Secretariat~M6");
        g.addVertex("Motijheel~M6");

        // Sequential connections (main line)
        g.addEdge("Uttara_North~M6", "Uttara_Center~M6", 1);
        g.addEdge("Uttara_Center~M6", "Uttara_South~M6", 1);
        g.addEdge("Uttara_South~M6", "Pallabi~M6", 2);
        g.addEdge("Pallabi~M6", "Mirpur_11~M6", 1);
        g.addEdge("Mirpur_11~M6", "Mirpur_10~M6", 1);
        g.addEdge("Mirpur_10~M6", "Kazipara~M6", 2);
        g.addEdge("Kazipara~M6", "Shewrapara~M6", 1);
        g.addEdge("Shewrapara~M6", "Agargaon~M6", 2);
        g.addEdge("Agargaon~M6", "Bijoy_Sarani~M6", 1);
        g.addEdge("Bijoy_Sarani~M6", "Farmgate~M6", 1);
        g.addEdge("Farmgate~M6", "Karwan_Bazar~M6", 1);
        g.addEdge("Karwan_Bazar~M6", "Shahbag~M6", 1);
        g.addEdge("Shahbag~M6", "Dhaka_University~M6", 1);
        g.addEdge("Dhaka_University~M6", "Bangladesh_Secretariat~M6", 1);
        g.addEdge("Bangladesh_Secretariat~M6", "Motijheel~M6", 2);
    }
};

// Helper function to clear input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Main application
int main() {
    MetroGraph metro("metro_data.txt");
    MetroGraph::createDefaultMetroMap(metro);

    cout << "\n\n\t\t\t\tWELCOME TO THE DHAKA METRO APP" << endl;
    cout << "\t\t\t\t=============================" << endl;
    cout << "\nTotal Stations: " << Station::getTotalStations() << endl;
    cout << metro << endl;

    while (true) {
        cout << "\n\n\t*************************************************************************" << endl;
        cout << "\t*                        ~LIST OF ACTIONS~                              *" << endl;
        cout << "\t*************************************************************************" << endl;
        cout << "\t*  1. LIST ALL STATIONS                                                 *" << endl;
        cout << "\t*  2. SHOW THE METRO MAP                                                *" << endl;
        cout << "\t*  3. GET SHORTEST DISTANCE                                             *" << endl;
        cout << "\t*  4. GET SHORTEST TIME                                                 *" << endl;
        cout << "\t*  5. GET SHORTEST PATH (DISTANCE WISE)                                 *" << endl;
        cout << "\t*  6. GET SHORTEST PATH (TIME WISE)                                     *" << endl;
        cout << "\t*  7. GET FARE (DISTANCE WISE)                                          *" << endl;
        cout << "\t*  8. GET FARE (TIME WISE)                                              *" << endl;
        cout << "\t*  9. SAVE DATA TO FILE                                                 *" << endl;
        cout << "\t* 10. EXIT                                                              *" << endl;
        cout << "\t*************************************************************************" << endl;

        int choice;
        cout << "\nENTER YOUR CHOICE (1-10): ";

        if (!(cin >> choice)) {
            cout << "\nINVALID INPUT! Please enter a number." << endl;
            clearInputBuffer();
            continue;
        }

        if (choice == 10) {
            cout << "\n===============================================" << endl;
            cout << "   Thank you for using Dhaka Metro App!" << endl;
            cout << "===============================================\n" << endl;
            break;
        }

        switch (choice) {
            case 1:
                metro.displayStations();
                break;

            case 2:
                metro.display();
                break;

            case 3: {
                string src, dest;
                cout << "\nENTER SOURCE STATION: ";
                clearInputBuffer();
                getline(cin, src);
                cout << "ENTER DESTINATION STATION: ";
                getline(cin, dest);

                if (!metro.containsVertex(src) || !metro.containsVertex(dest) || !metro.hasPath(src, dest)) {
                    cout << "\n*** INVALID INPUT! Please check station names. ***" << endl;
                } else {
                    int dist = metro.dijkstra(src, dest, false);
                    if (dist != -1) {
                        cout << "\n===============================================" << endl;
                        cout << "SHORTEST DISTANCE: " << dist << " KM" << endl;
                        cout << "===============================================" << endl;
                    } else {
                        cout << "\nPATH NOT FOUND!" << endl;
                    }
                }
                break;
            }

            case 4: {
                string src, dest;
                cout << "\nENTER SOURCE STATION: ";
                clearInputBuffer();
                getline(cin, src);
                cout << "ENTER DESTINATION STATION: ";
                getline(cin, dest);

                if (!metro.containsVertex(src) || !metro.containsVertex(dest) || !metro.hasPath(src, dest)) {
                    cout << "\n*** INVALID INPUT! Please check station names. ***" << endl;
                } else {
                    int time = metro.dijkstra(src, dest, true);
                    if (time != -1) {
                        cout << "\n===============================================" << endl;
                        cout << "SHORTEST TIME: " << time / 60 << " MINUTES" << endl;
                        cout << "===============================================" << endl;
                    } else {
                        cout << "\nPATH NOT FOUND!" << endl;
                    }
                }
                break;
            }

            case 5: {
                string src, dest;
                cout << "\nENTER SOURCE STATION: ";
                clearInputBuffer();
                getline(cin, src);
                cout << "ENTER DESTINATION STATION: ";
                getline(cin, dest);

                if (!metro.containsVertex(src) || !metro.containsVertex(dest) || !metro.hasPath(src, dest)) {
                    cout << "\n*** INVALID INPUT! Please check station names. ***" << endl;
                } else {
                    string result = metro.getMinimumDistance(src, dest);
                    if (!result.empty()) {
                        stringstream ss(result);
                        string token;
                        vector<string> path;
                        while (ss >> token) {
                            path.push_back(token);
                        }

                        cout << "\n===============================================" << endl;
                        cout << "SOURCE: " << src << endl;
                        cout << "DESTINATION: " << dest << endl;
                        cout << "DISTANCE: " << path[path.size() - 1] << " KM" << endl;
                        cout << "\nPATH:" << endl;
                        cout << "-----------------------------------------------" << endl;
                        for (size_t i = 0; i < path.size() - 1; i++) {
                            cout << path[i] << endl;
                        }
                        cout << "===============================================" << endl;
                    } else {
                        cout << "\nPATH NOT FOUND!" << endl;
                    }
                }
                break;
            }

            case 6: {
                string src, dest;
                cout << "\nENTER SOURCE STATION: ";
                clearInputBuffer();
                getline(cin, src);
                cout << "ENTER DESTINATION STATION: ";
                getline(cin, dest);

                if (!metro.containsVertex(src) || !metro.containsVertex(dest) || !metro.hasPath(src, dest)) {
                    cout << "\n*** INVALID INPUT! Please check station names. ***" << endl;
                } else {
                    string result = metro.getMinimumTime(src, dest);
                    if (!result.empty()) {
                        stringstream ss(result);
                        string token;
                        vector<string> path;
                        while (ss >> token) {
                            path.push_back(token);
                        }

                        cout << "\n===============================================" << endl;
                        cout << "SOURCE: " << src << endl;
                        cout << "DESTINATION: " << dest << endl;
                        cout << "TIME: " << path[path.size() - 1] << " MINUTES" << endl;
                        cout << "\nPATH:" << endl;
                        cout << "-----------------------------------------------" << endl;
                        for (size_t i = 0; i < path.size() - 1; i++) {
                            cout << path[i] << endl;
                        }
                        cout << "===============================================" << endl;
                    } else {
                        cout << "\nPATH NOT FOUND!" << endl;
                    }
                }
                break;
            }

            case 7: {
                string src, dest;
                cout << "\nENTER SOURCE STATION: ";
                clearInputBuffer();
                getline(cin, src);
                cout << "ENTER DESTINATION STATION: ";
                getline(cin, dest);

                if (!metro.containsVertex(src) || !metro.containsVertex(dest) || !metro.hasPath(src, dest)) {
                    cout << "\nINVALID INPUT OR NO PATH EXISTS!" << endl;
                } else {
                    vector<int> fareInfo = metro.getMinimumFare(src, dest, true);
                    cout << "\nNUMBER OF STATIONS: " << fareInfo[0] << endl;
                    cout << "CALCULATED FARE: Tk " << fareInfo[1] << endl;
                }
                break;
            }

            case 8: {
                string src, dest;
                cout << "\nENTER SOURCE STATION: ";
                clearInputBuffer();
                getline(cin, src);
                cout << "ENTER DESTINATION STATION: ";
                getline(cin, dest);

                if (!metro.containsVertex(src) || !metro.containsVertex(dest) || !metro.hasPath(src, dest)) {
                    cout << "\nINVALID INPUT OR NO PATH EXISTS!" << endl;
                } else {
                    vector<int> fareInfo = metro.getMinimumFare(src, dest, false);
                    cout << "\nNUMBER OF STATIONS: " << fareInfo[0] << endl;
                    cout << "CALCULATED FARE: Tk " << fareInfo[1] << endl;
                }
                break;
            }

            case 9:
                metro.saveToFile();
                break;

            default:
                cout << "\nINVALID CHOICE! Please enter a number between 1-10." << endl;
                clearInputBuffer();
                break;
        }
    }

    cout << "\nFinal Statistics:" << endl;
    cout << "Total Stations Created: " << Station::getTotalStations() << endl;
    cout << "Total Graphs Created: " << Graph::getGraphCount() << endl;

    return 0;
}
