#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <set>

using namespace std;
void equationParser(string &equationStr, vector<string> &LHS, vector<string> &RHS);
void termParser(string &termStr, double &constant, string &variable);
void operationParser(string &operationStr, vector<string> &operationParams);
double calculateDeterminant(vector<vector<double>> &coffVec);
void DeterminantOfVariable(vector<vector<double>> &vecOfReplacedVar,vector<map<string,double>> &equmap,string &column,double &result);
unsigned int NumOfEquations;

int main() {

  // read equations
  vector<string> givenEquationStr ;
  cin >> ::NumOfEquations ;
  for(unsigned int i = 0; i < ::NumOfEquations; i++) {
    string equation;
    cin >> equation  ;
    givenEquationStr.push_back(equation);
  }
  //containers
  vector<vector<double>> variableReplacing(::NumOfEquations);
  vector<map<string, double>> equMap(::NumOfEquations);
  vector<vector<double>> coffVec(::NumOfEquations);
  set<string> uniqVars;
  for(unsigned int i = 0; i < ::NumOfEquations; i++) {
    vector<vector<string>> LHS(::NumOfEquations);
    vector<vector<string>> RHS(::NumOfEquations);
    equationParser(givenEquationStr[i], LHS[i], RHS[i]);
    for (unsigned int j = 0; j < LHS[i].size(); j++) {
      string variable;
      double constant;
      termParser(LHS[i][j], constant, variable);
      if(variable == "") equMap[i][variable] -= constant;
      else equMap[i][variable] += constant;
    }
    for (unsigned int j = 0; j < RHS[i].size(); j++) {
      string variable;
      double constant;
      termParser(RHS[i][j], constant, variable);
      if(variable == "") equMap[i][variable] += constant;
      else equMap[i][variable] -= constant;
    }
  }
  // remove zero variables
  for (auto i = equMap.begin(); i != equMap.end(); i++)
  for (auto j = (*i).begin(); j != (*i).end(); j++){
      if(j->second == 0 && j->first == "") continue;
      if(j->second == 0) i->erase(j);
    }

  // get unique variables
  for (auto i = equMap.begin(); i != equMap.end(); i++)
    for (auto j = (*i).begin(); j != (*i).end(); j++)
      uniqVars.insert(j->first);
  uniqVars.erase(uniqVars.begin());

  // make equations balnced
  for (auto i = equMap.begin(); i != equMap.end(); i++)
    for (auto j = (*i).begin(); j != (*i).end(); j++)
      for (auto k = uniqVars.begin(); k != uniqVars.end(); k++) {
        if ((*i).count(*k) == 0) (*i)[*k] = 0;
        if ((*j).first != "" && uniqVars.find((*j).first) == uniqVars.end()) {
          i->erase(i->find(j->first));
          break;
        }
    }

  // get coffs
  for(int i = 0; i < equMap.size(); i++) {
    auto j = equMap[i].begin();
    for(j++; j != equMap[i].end(); j++)
      coffVec[i].push_back(j->second);
  }

  // start
  string operationStr;
  vector<string> operationParams;
  getline(cin, operationStr);
  operationParser(operationStr, operationParams);
  while(operationParams[0] != "quit") {
    if(operationParams[0] == "num_vars")
      cout << uniqVars.size() << endl;
    if(operationParams[0] == "equation") {
      int i = atoi(operationParams[1].c_str());
      auto j = equMap[i - 1].begin();
      bool isFirst = true;
      for (j++; j != equMap[i - 1].end(); j++) {
        if((*j).second == 0) continue;
        if((*j).second > 0 && !isFirst) cout << "+";
        cout << (*j).second << (*j).first;
        isFirst = false;
      }
      cout << "=" << equMap[i - 1][""] << endl;
    }
    if(operationParams[0] == "column") {
      string column = operationParams[1];
      for (auto i = equMap.begin(); i != equMap.end(); i++)
        cout << (*i)[column] << endl;
    }
    if(operationParams[0] == "add") {
      int i = atoi(operationParams[1].c_str());
      int j = atoi(operationParams[2].c_str());
      bool isFirst = true;
      double sum_rhs;
      for (auto it = equMap[i - 1].begin(); it != equMap[i - 1].end(); it++) {
        double sum = it->second + equMap[j - 1][it->first];
        if(it->first == "") {
          sum_rhs = sum;
          continue;
        }
        if(sum != 0) {
          if(sum > 0 && !isFirst) cout << "+";
          cout << sum << it->first;
          isFirst = false;
        }
      }
      cout << "=" << sum_rhs << endl;
    }
    if(operationParams[0] == "subtract") {
      int i = atoi(operationParams[1].c_str());
      int j = atoi(operationParams[2].c_str());
      bool isFirst = true;
      double sum_rhs;
      for (auto it = equMap[i - 1].begin(); it != equMap[i - 1].end(); it++) {
        double sum = it->second - equMap[j - 1][it->first];
        if(it->first == "") {
          sum_rhs = sum;
          continue;
        }
        if(sum != 0) {
          if(sum > 0 && !isFirst) cout << "+";
          cout << sum << it->first;
          isFirst = false;
        }
      }
      cout << "=" << sum_rhs << endl;
    }
    if(operationParams[0] == "substitute") {
      string variable = operationParams[1];
      int i = atoi(operationParams[2].c_str()) - 1;
      int j = atoi(operationParams[3].c_str()) - 1;
      double multipler = equMap[i][variable];
      double divider = equMap[j][variable];
      double correct_coff = -1 * multipler / divider;
      bool isFirst = true;
      double constant_rhs;
      for (auto it = equMap[i].begin(); it != equMap[i].end(); it++) {
        double new_coff = (correct_coff * equMap[j][it->first]) + equMap[i][it->first];
        if(it->first == "") {
          constant_rhs = new_coff;
          continue;
        }
        if(new_coff != 0) {
          if(new_coff > 0 && !isFirst) cout << "+";
          cout << new_coff << it->first;
          isFirst = false;
        }
      }
      cout << "=" << constant_rhs << endl;
    }
    if(operationParams[0] == "D" && operationParams.size() == 1) {
      for (int i = 0; i < equMap.size(); i++) {
        for (auto it = equMap[i].begin(); it != equMap[i].end(); it++)
          if(it->first != "")
            cout << it->second << "\t";
        cout << endl;
      }
    }
    if(operationParams[0] == "D" && operationParams.size() == 2) {
      string column = operationParams[1];
      for (int i = 0; i < equMap.size(); i++) {
        for (auto it = equMap[i].begin(); it != equMap[i].end(); it++){
          if(it->first != "" && it->first != column)
            cout << it->second << "\t";
          if(it->first == column)
            cout << equMap[i][""] << "\t";
        }
        cout << endl;
      }
    }
    if(operationParams[0] == "D_value")
      cout <<    calculateDeterminant(coffVec) << endl;

     if(operationParams[0] == "solve"){
      if(uniqVars.size() > ::NumOfEquations)
        cout << "No solution" << endl ;
         double TheMainDeterminant =  calculateDeterminant(coffVec);
          for(auto k = uniqVars.begin() ; k != uniqVars.end() ; k++ ){
          double result ;
          string column = *k ;
          DeterminantOfVariable(variableReplacing, equMap , column ,result);
          for (int i = 0; i < ::NumOfEquations; ++i)
            variableReplacing[i].clear();
          for(int i = 0 ; i<1 ; i++)
            cout << column << "=" << result/TheMainDeterminant << endl;
      }
    }
    getline(cin, operationStr);
    operationParams.clear();
    operationParser(operationStr, operationParams);
  }
  return 0;
}

void equationParser(string &equationStr, vector<string> &LHS, vector<string> &RHS) {
  string term;
  bool isRHS = false;
  for(unsigned int i = 0; i < equationStr.length(); i++) {
    if(equationStr[i] == '-' || equationStr[i] == '+' || equationStr[i] == '=') {
      if(isRHS) RHS.push_back(term);
      else LHS.push_back(term);
      if(equationStr[i] == '-' || equationStr[i] == '+')
        term = equationStr[i];
      else term = "";
      if(equationStr[i] == '=')
        isRHS = true;
    } else term += equationStr[i];
  }
  RHS.push_back(term);
}

void termParser(string &termStr, double &constant, string &variable) {
  variable = "";
  string constantStr = "";
  bool isConstant = true;
  for(int i = 0; i < termStr.length(); i++){
    if(
       (
        (int(termStr[i]) <= int('9') && int(termStr[i]) >= int('0'))
        || termStr[i] == '.'
        || termStr[i] == '-'
        || termStr[i] == '+'
       )
      && (isConstant)
    ) {
      constantStr += termStr[i];
    } else {
      isConstant = false; variable += termStr[i];
    }
  }
  if(constantStr.length() == 0) constant = 1;
  else if(constantStr.length() == 1 && constantStr[0] == '-') constant = -1;
  else if(constantStr.length() == 1 && constantStr[0] == '+') constant = 1;
  else constant = atof(constantStr.c_str());
  // cout << ">> coff = " << constant << " variable = " << variable << endl;
}

void operationParser(string &operationStr, vector<string> &operationParams) {
  string param = "";
  for (unsigned int i = 0; i < operationStr.length(); i++)
    if(operationStr[i] == ' ') {
      operationParams.push_back(param);
      param = "";
    }
    else param += operationStr[i];
  operationParams.push_back(param);
}

double calculateDeterminant(vector<vector<double>> &coffVec) {
  double result = 0;
  int rows = coffVec.size();
  int cols = coffVec[0].size();
  if(rows == 2 && cols == 2)
    return (coffVec[0][0] * coffVec[1][1]) - (coffVec[0][1] * coffVec[1][0]);
  int sign;
  for (int a = 0; a < coffVec[0].size(); a++) {
    vector<vector<double>> coffVecNew(rows - 1);
    int x = 0, y = 0;
    for (int l = 1; l < coffVec.size(); l++) {
      for (int m = 0; m < coffVec[l].size(); m++) {
        if(a == m) continue;
        coffVecNew[x].push_back(coffVec[l][m]);
      }
      x++;
      y = 0;
    }
    sign = (a % 2 == 0) ? 1 : -1;
    result += sign * coffVec[0][a] * calculateDeterminant(coffVecNew);
  }
  return result;
}

void DeterminantOfVariable(vector<vector<double>> &vecOfReplacedVar ,vector<map<string,double>> &equmap,string &column,double &result){
      for (int i = 0; i < equmap.size(); i++) {
        for (auto it = equmap[i].begin(); it != equmap[i].end(); it++){
          if(it->first != "" && it->first != column)
          vecOfReplacedVar[i].push_back(it->second) ;
          if(it->first == column)
             vecOfReplacedVar[i].push_back(equmap[i][""]) ;
        }
      }
      result =  calculateDeterminant(vecOfReplacedVar) ;
}
