#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>

using namespace std;


class TicTacToe {
private:
    vector<vector<int>> board;

public:
    int player;
    TicTacToe() {
        board = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
        player = 1;
    }
    vector<vector<int>> getBoard() { return board; }
    vector<pair<int, int>> actions() {
        vector<pair<int, int>> actions;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == 0) {
                    actions.push_back(make_pair(i, j));
                }
            }
        }
        return actions;
    }
    TicTacToe result(pair<int, int> action) {
        TicTacToe new_state = *this;
        if (board[action.first][action.second] == 0) {
            new_state.board[action.first][action.second] = player;
            new_state.player = 3 - player;
        }
        else {
            cout << "That square is already filled!" << endl;
        }
        return new_state;
    }
    int terminal() {
        for (int i = 0; i < 3; i++) {
            if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != 0) {
                return board[i][0];
            }
            if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != 0) {
                return board[0][i];
            }
        }
        if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != 0) {
            return board[0][0];
        }
        if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != 0) {
            return board[0][2];
        }
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == 0) {
                    return 0;
                }
            }
        }
        return -1;
    }

};

class QPlayer {
private:
    unordered_map<string, double> q;
    double epsilon;
    double alpha;
    double gamma;
public:
    QPlayer(double alpha, double epsilon, double gamma) :
        alpha(alpha), epsilon(epsilon), gamma(gamma), q() {}

    double getMaxQ(TicTacToe state) {
        vector<double> qs;
        for (auto action : state.actions()) {
            stringstream key;
            auto board = state.getBoard();
            key << board[0][0] << board[0][1] << board[0][2] << board[1][0] << board[1][1]
                << board[1][2] << board[2][0] << board[2][1] << board[2][2] << action.first << action.second;
            qs.push_back(q[key.str()]);
        }
        return *max_element(qs.begin(), qs.end());
    }
    pair<int, int> move(TicTacToe state) {
        if ((double)rand() / RAND_MAX < epsilon) {
            return state.actions()[rand() % state.actions().size()];
        }
        else {
            vector<double> qs;
            for (auto action : state.actions()) {
                stringstream key;
                auto board = state.getBoard();
                key << board[0][0] << board[0][1] << board[0][2] << board[1][0] << board[1][1] << board[1][2] << board[2][0] << board[2][1] << board[2][2] << action.first << action.second;
                qs.push_back(q[key.str()]);
            }
            auto maxQ = *max_element(qs.begin(), qs.end());
            vector<pair<int, int>> best;
            for (int i = 0; i < state.actions().size(); i++) {
                stringstream key;
                auto board = state.getBoard();
                key << board[0][0] << board[0][1] << board[0][2] << board[1][0] << board[1][1] << board[1][2] << board[2][0] << board[2][1] << board[2][2] << state.actions()[i].first << state.actions()[i].second;
                if (q[key.str()] == maxQ) {
                    best.push_back(state.actions()[i]);
                }
            }
            return best[rand() % best.size()];
        }
    }

    double getQ(TicTacToe state, pair<int, int> action) {
        stringstream key;
        auto board = state.getBoard();
        key << board[0][0] << board[0][1] << board[0][2] << board[1][0]
            << board[1][1] << board[1][2] << board[2][0] << board[2][1]
            << board[2][2] << action.first << action.second;
        if (q.find(key.str()) == q.end()) {
            q[key.str()] = 0;
        }
        return q[key.str()];
    }


    void learnQ(TicTacToe prev_state, pair<int, int> action, double reward, TicTacToe new_state) {
        double prev_q = getQ(prev_state, action);
        double maxQ = getMaxQ(new_state);
        double new_q = prev_q + alpha * (reward + gamma * maxQ - prev_q);
        stringstream key;
        auto prev_board = prev_state.getBoard();
        key << prev_board[0][0] << prev_board[0][1] << prev_board[0][2] << prev_board[1][0]
            << prev_board[1][1] << prev_board[1][2] << prev_board[2][0] << prev_board[2][1]
            << prev_board[2][2] << action.first << action.second;
        q[key.str()] = new_q;
    }
};

void print_board(vector<vector<int>> board) {
    cout << endl;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 1) {
                cout << "X ";
            }
            else if (board[i][j] == 2) {
                cout << "O ";
            }
            else {
                cout << "- ";
            }

        }
        cout << endl;
    }
}

int main() {
    double alpha = 0.1;
    double epsilon = 0.1;
    double gamma = 0.9;
    unordered_map<string, double> q;
    cout << "Waiting for the agent to train.." << endl;
    TicTacToe state;
    QPlayer qp(alpha, epsilon, gamma);
    int episodes = 10000;
    while (episodes--) {
        state = TicTacToe();
        auto action = qp.move(state);
        while (true) {
            auto new_state = state.result(action);
            int winner = new_state.terminal();
            if (winner != 0) {
                break;
            }
            qp.learnQ(state, action, 0, new_state);
            state = new_state;
            action = qp.move(state);
        }
    }

    state = TicTacToe();
    while (true) {
        print_board(state.getBoard());

        if (state.player == 1) {
            int row, col;
            cout << "Human player turn, insert row and col: " << endl;
            cout << "row = ";
            cin >> row;
            cout << "col = ";
            cin >> col;
            pair<int, int> action = make_pair(row, col);
            auto new_state = state.result(action);
            state = new_state;
        }
        else {
            auto action = qp.move(state);
            auto new_state = state.result(action);
            qp.learnQ(state, action, 0, new_state);
            state = new_state;
        }
        int winner = state.terminal();
        if (winner != 0) {
            print_board(state.getBoard());
            cout << "Player " << winner << " wins!" << endl;
            break;
        }
    }
    return 0;
}
