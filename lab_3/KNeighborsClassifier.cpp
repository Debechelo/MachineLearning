#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>

using namespace std;

class Passenger
{
public:
    int PassengerId, Survived, Pclass, SibSp, Parch;
    double Age;
    double Fare;
    string Name, Sex, Ticket, Cabin, Embarked;
    Passenger() {}
    static Passenger Parse(string);
};

vector<Passenger> readData(string fileName);

double Distance(Passenger &a, Passenger &b)
{
    double d = 0;
    d += abs(a.Pclass - b.Pclass);
    d += a.Sex != b.Sex;
    d += fabs(a.Age - b.Age);
    d += abs(a.SibSp - b.SibSp);
    d += abs(a.Parch - b.Parch);
    d += fabs(a.Fare - b.Fare);
    d += a.Embarked != b.Embarked;
    return d;
}

bool comparison(const pair<double,int> &a, const pair<double,int> &b)
{ return a.first<b.first; }

int main()
{
    int K = 5;
    vector<Passenger> data = readData("data.csv");
    cout << "data.size() = "<< data.size() << " " <<"\n";
    //иногда оказывается более K соседей на одинаковом близком расстоянии, из-за этого точность - случайна. Считаем минимально и максимально возможные
    double accuracy = 0;
    vector<pair<double,int> > dists(data.size());
    for(int i = 0; i < data.size(); i++)
    {
        //выполняем предсказание для i-того пассажира
        for(int j = 0; j < data.size(); j++)
            if (i != j) dists[j] = pair<double,int>(Distance(data[i], data[j]), data[j].Survived);
            else dists[j] = pair<double,int>(1e100, 0); //чтобы этот же пассажир никогда не попадал в число своих соседей
        sort(dists.begin(), dists.end(), comparison);
        double radius = dists[K-1].first; // радиус окружности из которой берем соседей
        double prediction = 0;
        for (int k = 0; k < K; k++) prediction += dists[k].second;
        //обрабатываем соседей с тем же радиусом, что и у К-того (К-1 в С++)
        int k = K;
        while (dists[k].first == radius && k<data.size()) prediction += dists[k++].second;
        accuracy += data[i].Survived == round(prediction/k); // k - на самом деле >=K, k>K когда несколько соседей на одном и том же расстоянии
    }
    accuracy /= data.size();
    cout << "Accuracy of " << K << "-NN model is " << accuracy*100 << "%\n";
}

vector<string> splitString(string s, char delim)
{
    vector<string> elems;
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) elems.push_back(item);
    if (s[s.size()-1] == delim) elems.push_back("");
    return elems;
}
template< typename T >
inline T convert(const string& str)
{
    istringstream iss(str);
    T n = -1;
    iss >> ws >> n >> ws;
    if(!iss.eof()) return -1;
    return n;
}

Passenger Passenger::Parse(string s)
{
    Passenger p;
    int nameStart = s.find('"');
    int nameEnd = s.rfind('"');
    p.Name = s.substr(nameStart+1, nameEnd-nameStart-1);
    vector<string> parts1 = splitString(s.substr(0, nameStart-1), ',');
    vector<string> parts2 = splitString(s.substr(nameEnd+2), ',');
    p.PassengerId = convert<int>(parts1[0]);
    p.Survived = convert<int>(parts1[1]);
    p.Pclass = convert<int>(parts1[2]);
    p.Sex = parts2[0];
    p.Age = convert<double>(parts2[1]);
    p.SibSp = convert<int>(parts2[2]);
    p.Parch = convert<int>(parts2[3]);
    p.Ticket = parts2[4];
    p.Fare = convert<double>(parts2[5]);
    p.Cabin = parts2[6];
    p.Embarked = parts2[7];
    return p;
}

vector<Passenger> readData(string fileName)
{
    vector<Passenger> data;
    ifstream infile(fileName.c_str());
    string line;
    getline(infile, line);//пропускаем строку заголовков
    while (getline(infile, line))
        if (line.size() > 0)
        {
            Passenger p = Passenger::Parse(line);
            if (p.Age >= 0) data.push_back(p);
        }
    return data;
}
