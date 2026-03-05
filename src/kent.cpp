#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QMessageBox>
#include <QInputDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QStackedWidget>
#include <QTabWidget>
#include <QFormLayout>
#include <QGroupBox>
#include <QIntValidator>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QVariant>

using namespace std;

// 定义列车信息结构体
struct Train {
	string trainNumber;
	vector<string> stations;
	vector<string> arrivalTimes;
	vector<vector<int>> segmentAvailableSeats; // 二维数组：segmentAvailableSeats[i][j] 表示从站i到站j的余票
	vector<vector<int>> priceMatrix; // 二维数组：priceMatrix[i][j] 表示从站i到站j的票价
	
	Train(string tn, vector<string> sta, vector<string> arr, vector<vector<int>> sas, vector<vector<int>> pm = {})
	: trainNumber(tn), stations(sta), arrivalTimes(arr), segmentAvailableSeats(sas), priceMatrix(pm) {}
};

// 定义用户结构体
struct User {
	int id;
	string phoneNumber;  // 将username改为phoneNumber
	string password;
	string name;
	string idNumber;
	vector<Train> trips;
	double balance; // 账户余额
	
	User(string phone, string pwd, string nm, string id_num, double bal = 3000.0, int user_id = -1)
	: id(user_id), phoneNumber(phone), password(pwd), name(nm), idNumber(id_num), balance(bal) {}
};

// 定义管理员结构体
struct Admin {
	int id;
	string username;
	string password;
	string name;
	
	Admin(string un, string pwd, string nm, int admin_id = -1)
	: id(admin_id), username(un), password(pwd), name(nm) {}
};

// 全局变量
vector<User> users;
vector<Admin> admins;
vector<Train> trains;
vector<string> suspendedTrains; // 停开的列车车次
string currentStartStation;
string currentEndStation;
string currentDepartureTimeFilter;

// 数据库连接
QSqlDatabase db;

// 数据库相关函数声明
bool initDatabase();
bool loadUsersFromDB();
bool saveUsersToDB();
bool loadAdminsFromDB();
bool saveAdminsToDB();
bool loadTrainsFromDB();
bool saveTrainsToDB();
bool loadSuspendedTrainsFromDB();
bool saveSuspendedTrainsToDB();

// 数据库表名常量
const string DB_NAME = "railway_system.db";

// 全局界面变量
User* currentUser = nullptr;
Admin* currentAdmin = nullptr;
QStackedWidget* stackedWidget = nullptr;
QWidget* startMenuWidget = nullptr;
QWidget* loginWidget = nullptr;
QWidget* registerWidget = nullptr;
QWidget* adminLoginWidget = nullptr;
QWidget* adminRegisterWidget = nullptr;
QWidget* mainMenuWidget = nullptr;
QWidget* adminMenuWidget = nullptr;
QTableWidget* ticketTable = nullptr;
QTableWidget* myTripsTable = nullptr;
QTableWidget* adminTrainTable = nullptr;
QLabel* balanceLabel = nullptr;
QLineEdit* rechargeAmountEdit = nullptr;

// 模拟 MD5 哈希函数（简化版）
string md5(string input) {
	return input;
}

// 验证手机号格式
bool isValidPhoneNumber(const string& phone) {
	if (phone.length() != 11) return false;
	for (char c : phone) {
		if (!isdigit(c)) return false;
	}
	return true;
}

// 验证密码格式
bool isValidPassword(const string& password) {
	if (password.length() < 8) return false;
	
	bool hasUpper = false, hasLower = false, hasDigit = false;
	for (char c : password) {
		if (isupper(c)) hasUpper = true;
		else if (islower(c)) hasLower = true;
		else if (isdigit(c)) hasDigit = true;
	}
	
	return hasUpper && hasLower && hasDigit;
}

// 数据库初始化函数
bool initDatabase() {
	// 创建数据库连接
	db = QSqlDatabase::addDatabase("QSQLITE");
	// 为了调试方便，将数据库放在当前目录
	db.setDatabaseName(QString::fromStdString(DB_NAME));
	
	if (!db.open()) {
		cout << "数据库连接失败: " << db.lastError().text().toStdString() << endl;
		return false;
	}
	
	cout << "数据库连接成功: " << db.databaseName().toStdString() << endl;
	
	QSqlQuery query;
	
	// 创建用户表
	QString createUsersTable = R"(
		CREATE TABLE IF NOT EXISTS users (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			phone_number TEXT UNIQUE NOT NULL,
			password TEXT NOT NULL,
			name TEXT NOT NULL,
			id_number TEXT NOT NULL,
			balance REAL DEFAULT 3000.0
		)
	)";
	
	if (!query.exec(createUsersTable)) {
		cout << "创建用户表失败: " << query.lastError().text().toStdString() << endl;
		return false;
	}
	
	// 创建管理员表
	QString createAdminsTable = R"(
		CREATE TABLE IF NOT EXISTS admins (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			username TEXT UNIQUE NOT NULL,
			password TEXT NOT NULL,
			name TEXT NOT NULL
		)
	)";
	
	if (!query.exec(createAdminsTable)) {
		cout << "创建管理员表失败: " << query.lastError().text().toStdString() << endl;
		return false;
	}
	
	// 创建列车表
	QString createTrainsTable = R"(
		CREATE TABLE IF NOT EXISTS trains (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			train_number TEXT UNIQUE NOT NULL,
			stations TEXT NOT NULL,
			arrival_times TEXT NOT NULL,
			segment_available_seats TEXT NOT NULL,
			price_matrix TEXT NOT NULL
		)
	)";
	
	if (!query.exec(createTrainsTable)) {
		cout << "创建列车表失败: " << query.lastError().text().toStdString() << endl;
		return false;
	}
	
	// 创建停开列车表
	QString createSuspendedTable = R"(
		CREATE TABLE IF NOT EXISTS suspended_trains (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			train_number TEXT UNIQUE NOT NULL
		)
	)";
	
	if (!query.exec(createSuspendedTable)) {
		cout << "创建停开列车表失败: " << query.lastError().text().toStdString() << endl;
		return false;
	}
	
	// 创建用户行程表
	QString createTripsTable = R"(
		CREATE TABLE IF NOT EXISTS user_trips (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			user_id INTEGER NOT NULL,
			train_number TEXT NOT NULL,
			start_station TEXT NOT NULL,
			end_station TEXT NOT NULL,
			departure_time TEXT NOT NULL,
			arrival_time TEXT NOT NULL,
			price INTEGER NOT NULL,
			FOREIGN KEY (user_id) REFERENCES users (id)
		)
	)";
	
	if (!query.exec(createTripsTable)) {
		cout << "创建用户行程表失败: " << query.lastError().text().toStdString() << endl;
		return false;
	}
	
	cout << "数据库表初始化完成" << endl;
	return true;
}

// 辅助函数：将字符串分割成向量
vector<string> split(const string& str, char delimiter) {
	vector<string> tokens;
	string token;
	istringstream tokenStream(str);
	while (getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

// 辅助函数：去除字符串首尾空格
string trim(const string& str) {
	size_t first = str.find_first_not_of(' ');
	if (string::npos == first) {
		return str;
	}
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, last - first + 1);
}

// 辅助函数：将字符串转换为小写
string toLowerCase(const string& str) {
	string lowerStr;
	for (char c : str) {
		lowerStr += tolower(c);
	}
	return lowerStr;
}

// 辅助函数：时间字符串转换为分钟数
int timeToMinutes(const string& timeStr) {
	string normalizedTime = timeStr;
	
	// 处理不同的时间格式
	if (timeStr.length() == 4 && timeStr[1] == ':') {
		// 格式 "3:45" -> "03:45"
		normalizedTime = "0" + timeStr;
	} else if (timeStr.length() == 3 && timeStr.find(':') == string::npos) {
		// 格式 "345" -> "03:45"
		if (timeStr.length() == 3) {
			normalizedTime = "0" + timeStr.substr(0, 1) + ":" + timeStr.substr(1, 2);
		}
	} else if (timeStr.length() == 4 && timeStr.find(':') == string::npos) {
		// 格式 "1345" -> "13:45"
		normalizedTime = timeStr.substr(0, 2) + ":" + timeStr.substr(2, 2);
	}
	
	if (normalizedTime.length() != 5 || normalizedTime[2] != ':') {
		return 0; // 无效时间格式，返回0
	}
	
	try {
		int hours = stoi(normalizedTime.substr(0, 2));
		int minutes = stoi(normalizedTime.substr(3, 2));
		return hours * 60 + minutes;
	} catch (const exception& e) {
		return 0; // 解析失败，返回0
	}
}

// 辅助函数：分钟数转换为时间字符串
string minutesToTime(int totalMinutes) {
	// 处理跨天情况
	if (totalMinutes >= 24 * 60) {
		totalMinutes = totalMinutes % (24 * 60);
	}
	if (totalMinutes < 0) {
		totalMinutes = (24 * 60) + (totalMinutes % (24 * 60));
	}
	
	int hours = totalMinutes / 60;
	int minutes = totalMinutes % 60;
	
	string hourStr = (hours < 10) ? "0" + to_string(hours) : to_string(hours);
	string minuteStr = (minutes < 10) ? "0" + to_string(minutes) : to_string(minutes);
	
	return hourStr + ":" + minuteStr;
}

// 辅助函数：从时刻表中获取指定方向的时间
vector<string> getDirectionalSchedule(const vector<string>& allTimes, size_t startIdx, size_t endIdx) {
	if (allTimes.empty()) {
		return allTimes;
	}
	
	// 计算站点数量（前一半是正向，后一半是反向）
	size_t numStations = allTimes.size() / 2;
	
	// 确保索引在有效范围内
	if (startIdx >= numStations || endIdx >= numStations) {
		// 如果索引超出范围，返回前半部分（正向时刻表）
		return vector<string>(allTimes.begin(), allTimes.begin() + numStations);
	}
	
	// 如果是正向行程（startIdx < endIdx），使用前半部分时间
	if (startIdx < endIdx) {
		return vector<string>(allTimes.begin(), allTimes.begin() + numStations);
	}
	// 如果是反向行程（startIdx > endIdx），使用后半部分时间
	else {
		return vector<string>(allTimes.begin() + numStations, allTimes.end());
	}
}

// 数据迁移函数：将文件数据导入到数据库
bool migrateDataFromFiles() {
	cout << "开始检查并迁移文件数据..." << endl;
	
	// 检查是否已有数据库数据
	QSqlQuery checkQuery;
	if (checkQuery.exec("SELECT COUNT(*) FROM trains")) {
		checkQuery.next();
		if (checkQuery.value(0).toInt() > 0) {
			cout << "数据库中已有数据，跳过迁移" << endl;
			return true;
		}
	}
	
	// 尝试导入列车数据
	ifstream trainFile("new_trains.txt");
	if (trainFile.is_open()) {
		cout << "发现列车数据文件，开始导入..." << endl;
		string line;
		while (getline(trainFile, line)) {
			vector<string> parts = split(line, ',');
			if (parts.size() >= 5) {
				string trainNumber = trim(parts[0]);
				
				QSqlQuery insertQuery;
				insertQuery.prepare("INSERT OR IGNORE INTO trains (train_number, stations, arrival_times, segment_available_seats, price_matrix) VALUES (?, ?, ?, ?, ?)");
				insertQuery.addBindValue(QString::fromStdString(trainNumber));
				insertQuery.addBindValue(QString::fromStdString(trim(parts[1])));
				insertQuery.addBindValue(QString::fromStdString(trim(parts[2])));
				insertQuery.addBindValue(QString::fromStdString(trim(parts[3])));
				insertQuery.addBindValue(QString::fromStdString(trim(parts[4])));
				
				if (!insertQuery.exec()) {
					cout << "导入列车 " << trainNumber << " 失败: " << insertQuery.lastError().text().toStdString() << endl;
				}
			}
		}
		trainFile.close();
		cout << "列车数据导入完成" << endl;
	}
	
	// 尝试导入用户数据（从未加密的文件）
	ifstream userFile("未加密txt文件/users.txt");
	if (userFile.is_open()) {
		cout << "发现用户数据文件，开始导入..." << endl;
		string line;
		while (getline(userFile, line) && !line.empty()) {
			string username = line;
			string password, name, idNumber, balanceStr;
			
			if (getline(userFile, password) && getline(userFile, name) && 
				getline(userFile, idNumber) && getline(userFile, balanceStr)) {
				
				double balance = 3000.0;
				try {
					balance = stod(balanceStr);
				} catch (...) {
					balance = 3000.0;
				}
				
				QSqlQuery insertQuery;
				insertQuery.prepare("INSERT OR IGNORE INTO users (phone_number, password, name, id_number, balance) VALUES (?, ?, ?, ?, ?)");
				insertQuery.addBindValue(QString::fromStdString(username));
				insertQuery.addBindValue(QString::fromStdString(password));
				insertQuery.addBindValue(QString::fromStdString(name));
				insertQuery.addBindValue(QString::fromStdString(idNumber));
				insertQuery.addBindValue(balance);
				
				if (!insertQuery.exec()) {
					cout << "导入用户 " << username << " 失败: " << insertQuery.lastError().text().toStdString() << endl;
				}
			}
		}
		userFile.close();
		cout << "用户数据导入完成" << endl;
	}
	
	// 尝试导入管理员数据
	ifstream adminFile("未加密txt文件/admins.txt");
	if (adminFile.is_open()) {
		cout << "发现管理员数据文件，开始导入..." << endl;
		string line;
		while (getline(adminFile, line) && !line.empty()) {
			string username = line;
			string password, name;
			
			if (getline(adminFile, password) && getline(adminFile, name)) {
				QSqlQuery insertQuery;
				insertQuery.prepare("INSERT OR IGNORE INTO admins (username, password, name) VALUES (?, ?, ?)");
				insertQuery.addBindValue(QString::fromStdString(username));
				insertQuery.addBindValue(QString::fromStdString(password));
				insertQuery.addBindValue(QString::fromStdString(name));
				
				if (!insertQuery.exec()) {
					cout << "导入管理员 " << username << " 失败: " << insertQuery.lastError().text().toStdString() << endl;
				}
			}
		}
		adminFile.close();
		cout << "管理员数据导入完成" << endl;
	}
	
	cout << "数据迁移完成" << endl;
	return true;
}

// 从数据库加载用户数据
bool loadUsersFromDB() {
	users.clear();
	
	QSqlQuery query;
	if (!query.exec("SELECT id, phone_number, password, name, id_number, balance FROM users")) {
		cout << "查询用户数据失败: " << query.lastError().text().toStdString() << endl;
		return false;
	}
	
	while (query.next()) {
		int userId = query.value(0).toInt();
		string phoneNumber = query.value(1).toString().toStdString();
		string password = query.value(2).toString().toStdString();
		string name = query.value(3).toString().toStdString();
		string idNumber = query.value(4).toString().toStdString();
		double balance = query.value(5).toDouble();
		
		User user(phoneNumber, password, name, idNumber, balance, userId);
		
		// 加载用户行程
		QSqlQuery tripQuery;
		tripQuery.prepare("SELECT train_number, start_station, end_station, departure_time, arrival_time, price FROM user_trips WHERE user_id = ?");
		tripQuery.addBindValue(userId);
		
		if (tripQuery.exec()) {
			while (tripQuery.next()) {
				string trainNumber = tripQuery.value(0).toString().toStdString();
				string startStation = tripQuery.value(1).toString().toStdString();
				string endStation = tripQuery.value(2).toString().toStdString();
				string departureTime = tripQuery.value(3).toString().toStdString();
				string arrivalTime = tripQuery.value(4).toString().toStdString();
				int price = tripQuery.value(5).toInt();
				
				// 创建简化的行程记录
				vector<string> tripStations = {startStation, endStation};
				vector<string> tripTimes = {departureTime, arrivalTime};
				vector<vector<int>> tripSeats(2, vector<int>(2, 0));
				vector<vector<int>> tripPrices(2, vector<int>(2, 0));
				tripPrices[0][1] = price;
				
				Train tripRecord(trainNumber, tripStations, tripTimes, tripSeats, tripPrices);
				user.trips.push_back(tripRecord);
			}
		}
		
		users.push_back(user);
	}
	
	cout << "从数据库加载了 " << users.size() << " 个用户" << endl;
	return true;
}

// 保存用户数据到数据库
bool saveUsersToDB() {
	QSqlQuery query;
	
	// 清空现有的用户数据和行程数据
	if (!query.exec("DELETE FROM user_trips") || !query.exec("DELETE FROM users")) {
		cout << "清空用户数据失败: " << query.lastError().text().toStdString() << endl;
		return false;
	}
	
	for (const auto& user : users) {
		// 插入用户数据
		query.prepare("INSERT INTO users (phone_number, password, name, id_number, balance) VALUES (?, ?, ?, ?, ?)");
		query.addBindValue(QString::fromStdString(user.phoneNumber));
		query.addBindValue(QString::fromStdString(user.password));
		query.addBindValue(QString::fromStdString(user.name));
		query.addBindValue(QString::fromStdString(user.idNumber));
		query.addBindValue(user.balance);
		
		if (!query.exec()) {
			cout << "插入用户数据失败: " << query.lastError().text().toStdString() << endl;
			continue;
		}
		
		// 获取插入的用户ID
		int userId = query.lastInsertId().toInt();
		
		// 插入用户行程数据
		for (const auto& trip : user.trips) {
			if (!trip.stations.empty() && trip.stations.size() >= 2) {
				string startStation = trip.stations[0];
				string endStation = trip.stations[trip.stations.size() - 1];
				string departureTime = trip.arrivalTimes.empty() ? "" : trip.arrivalTimes[0];
				string arrivalTime = trip.arrivalTimes.size() > 1 ? trip.arrivalTimes[trip.arrivalTimes.size() - 1] : departureTime;
				int price = 0;
				if (!trip.priceMatrix.empty() && !trip.priceMatrix[0].empty()) {
					price = trip.priceMatrix[0][trip.priceMatrix[0].size() - 1];
				}
				
				QSqlQuery tripQuery;
				tripQuery.prepare("INSERT INTO user_trips (user_id, train_number, start_station, end_station, departure_time, arrival_time, price) VALUES (?, ?, ?, ?, ?, ?, ?)");
				tripQuery.addBindValue(userId);
				tripQuery.addBindValue(QString::fromStdString(trip.trainNumber));
				tripQuery.addBindValue(QString::fromStdString(startStation));
				tripQuery.addBindValue(QString::fromStdString(endStation));
				tripQuery.addBindValue(QString::fromStdString(departureTime));
				tripQuery.addBindValue(QString::fromStdString(arrivalTime));
				tripQuery.addBindValue(price);
				
				if (!tripQuery.exec()) {
					cout << "插入行程数据失败: " << tripQuery.lastError().text().toStdString() << endl;
				}
			}
		}
	}
	
	cout << "保存了 " << users.size() << " 个用户到数据库" << endl;
	return true;
}

// 保存管理员数据到数据库
bool saveAdminsToDB() {
	QSqlQuery query;
	
	// 清空现有的管理员数据
	if (!query.exec("DELETE FROM admins")) {
		cout << "清空管理员数据失败: " << query.lastError().text().toStdString() << endl;
		return false;
	}
	
	for (const auto& admin : admins) {
		query.prepare("INSERT INTO admins (username, password, name) VALUES (?, ?, ?)");
		query.addBindValue(QString::fromStdString(admin.username));
		query.addBindValue(QString::fromStdString(admin.password));
		query.addBindValue(QString::fromStdString(admin.name));
		
		if (!query.exec()) {
			cout << "插入管理员数据失败: " << query.lastError().text().toStdString() << endl;
			return false;
		}
	}
	
	cout << "保存了 " << admins.size() << " 个管理员到数据库" << endl;
	return true;
}

// 从数据库加载管理员数据
bool loadAdminsFromDB() {
	admins.clear();
	
	QSqlQuery query;
	if (!query.exec("SELECT id, username, password, name FROM admins")) {
		cout << "查询管理员数据失败: " << query.lastError().text().toStdString() << endl;
		return false;
	}
	
	while (query.next()) {
		int adminId = query.value(0).toInt();
		string username = query.value(1).toString().toStdString();
		string password = query.value(2).toString().toStdString();
		string name = query.value(3).toString().toStdString();
		
		admins.emplace_back(username, password, name, adminId);
	}
	
	cout << "从数据库加载了 " << admins.size() << " 个管理员" << endl;
	return true;
}

// 保存停开列车数据到数据库
bool saveSuspendedTrainsToDB() {
	QSqlQuery query;
	
	// 清空现有的停开列车数据
	if (!query.exec("DELETE FROM suspended_trains")) {
		cout << "清空停开列车数据失败: " << query.lastError().text().toStdString() << endl;
		return false;
	}
	
	for (const auto& trainNumber : suspendedTrains) {
		query.prepare("INSERT INTO suspended_trains (train_number) VALUES (?)");
		query.addBindValue(QString::fromStdString(trainNumber));
		
		if (!query.exec()) {
			cout << "插入停开列车数据失败: " << query.lastError().text().toStdString() << endl;
			return false;
		}
	}
	
	cout << "保存了 " << suspendedTrains.size() << " 个停开列车到数据库" << endl;
	return true;
}

// 从数据库加载停开列车数据
bool loadSuspendedTrainsFromDB() {
	suspendedTrains.clear();
	
	QSqlQuery query;
	if (!query.exec("SELECT train_number FROM suspended_trains")) {
		cout << "查询停开列车数据失败: " << query.lastError().text().toStdString() << endl;
		return false;
	}
	
	while (query.next()) {
		string trainNumber = query.value(0).toString().toStdString();
		suspendedTrains.push_back(trainNumber);
	}
	
	cout << "从数据库加载了 " << suspendedTrains.size() << " 个停开列车" << endl;
	return true;
}

// 从数据库加载列车数据
bool loadTrainsFromDB() {
	trains.clear();
	
	QSqlQuery query;
	if (!query.exec("SELECT train_number, stations, arrival_times, segment_available_seats, price_matrix FROM trains")) {
		cout << "查询列车数据失败: " << query.lastError().text().toStdString() << endl;
		return false;
	}
	
	while (query.next()) {
		string trainNumber = query.value(0).toString().toStdString();
		
		// 解析站点
		string stationsStr = query.value(1).toString().toStdString();
		vector<string> stations = split(stationsStr, '|');
		
		// 解析到达时间
		string timesStr = query.value(2).toString().toStdString();
		vector<string> arrivalTimes = split(timesStr, '|');
		
		// 解析余票矩阵
		string seatsStr = query.value(3).toString().toStdString();
		vector<vector<int>> segmentAvailableSeats;
		vector<string> seatRows = split(seatsStr, '|');
		for (const string& row : seatRows) {
			vector<int> seats;
			vector<string> seatValues = split(row, ';');
			for (const string& value : seatValues) {
				try {
					seats.push_back(stoi(trim(value)));
				} catch (const exception& e) {
					seats.push_back(0);
				}
			}
			segmentAvailableSeats.push_back(seats);
		}
		
		// 解析票价矩阵
		string pricesStr = query.value(4).toString().toStdString();
		vector<vector<int>> priceMatrix;
		vector<string> priceRows = split(pricesStr, '|');
		for (const string& row : priceRows) {
			vector<int> prices;
			vector<string> priceValues = split(row, ';');
			for (const string& value : priceValues) {
				try {
					prices.push_back(stoi(trim(value)));
				} catch (const exception& e) {
					prices.push_back(0);
				}
			}
			priceMatrix.push_back(prices);
		}
		
		trains.emplace_back(trainNumber, stations, arrivalTimes, segmentAvailableSeats, priceMatrix);
	}
	
	cout << "从数据库加载了 " << trains.size() << " 个车次" << endl;
	return true;
}

// 保存列车数据到数据库
bool saveTrainsToDB() {
	QSqlQuery query;
	
	// 清空现有的列车数据
	if (!query.exec("DELETE FROM trains")) {
		cout << "清空列车数据失败: " << query.lastError().text().toStdString() << endl;
		return false;
	}
	
	for (const auto& train : trains) {
		// 序列化站点
		string stationsStr = "";
		for (size_t i = 0; i < train.stations.size(); ++i) {
			stationsStr += train.stations[i];
			if (i != train.stations.size() - 1) {
				stationsStr += "|";
			}
		}
		
		// 序列化到达时间
		string timesStr = "";
		for (size_t i = 0; i < train.arrivalTimes.size(); ++i) {
			timesStr += train.arrivalTimes[i];
			if (i != train.arrivalTimes.size() - 1) {
				timesStr += "|";
			}
		}
		
		// 序列化余票矩阵
		string seatsStr = "";
		for (size_t i = 0; i < train.segmentAvailableSeats.size(); ++i) {
			for (size_t j = 0; j < train.segmentAvailableSeats[i].size(); ++j) {
				seatsStr += to_string(train.segmentAvailableSeats[i][j]);
				if (j != train.segmentAvailableSeats[i].size() - 1) {
					seatsStr += ";";
				}
			}
			if (i != train.segmentAvailableSeats.size() - 1) {
				seatsStr += "|";
			}
		}
		
		// 序列化票价矩阵
		string pricesStr = "";
		for (size_t i = 0; i < train.priceMatrix.size(); ++i) {
			for (size_t j = 0; j < train.priceMatrix[i].size(); ++j) {
				pricesStr += to_string(train.priceMatrix[i][j]);
				if (j != train.priceMatrix[i].size() - 1) {
					pricesStr += ";";
				}
			}
			if (i != train.priceMatrix.size() - 1) {
				pricesStr += "|";
			}
		}
		
		// 插入到数据库
		query.prepare("INSERT INTO trains (train_number, stations, arrival_times, segment_available_seats, price_matrix) VALUES (?, ?, ?, ?, ?)");
		query.addBindValue(QString::fromStdString(train.trainNumber));
		query.addBindValue(QString::fromStdString(stationsStr));
		query.addBindValue(QString::fromStdString(timesStr));
		query.addBindValue(QString::fromStdString(seatsStr));
		query.addBindValue(QString::fromStdString(pricesStr));
		
		if (!query.exec()) {
			cout << "插入列车数据失败: " << query.lastError().text().toStdString() << endl;
			return false;
		}
	}
	
	cout << "保存了 " << trains.size() << " 个车次到数据库" << endl;
	return true;
}

// 全局界面变量
QLabel* passengerInfoLabel = nullptr;
QLabel* tripsPassengerNameLabel = nullptr;
QLabel* tripsPassengerPhoneLabel = nullptr;

// 更新余额显示
void updateBalanceDisplay() {
	if (currentUser && balanceLabel) {
		balanceLabel->setText(QString("当前余额: ¥%1").arg(QString::number(currentUser->balance, 'f', 2)));
	}
	
	// 更新乘车人信息显示
	if (currentUser) {
		QString passengerInfo = QString("乘车人：%1 (手机号：%2)")
			.arg(QString::fromStdString(currentUser->name))
			.arg(QString::fromStdString(currentUser->phoneNumber));
		
		if (passengerInfoLabel) {
			passengerInfoLabel->setText(passengerInfo);
		}
		
		// 更新我的行程页面的乘车人信息
		if (tripsPassengerNameLabel) {
			tripsPassengerNameLabel->setText(QString("乘车人：%1").arg(QString::fromStdString(currentUser->name)));
		}
		
		if (tripsPassengerPhoneLabel) {
			tripsPassengerPhoneLabel->setText(QString("手机号：%1").arg(QString::fromStdString(currentUser->phoneNumber)));
		}
	}
}

// 更新个人行程表
void updateMyTripsTable() {
	myTripsTable->setRowCount(0);
	
	if (!currentUser) return;
	
	int row = 0;
	for (const auto& trip : currentUser->trips) {
		if (!trip.stations.empty() && trip.stations.size() >= 2) {
			string startStation = trip.stations.front();
			string endStation = trip.stations.back();
			size_t startIdx = 0;
			size_t endIdx = trip.stations.size() - 1;
			
			// 获取票价 - 修复双向支持后的票价计算
			int price = 0;
			if (!trip.priceMatrix.empty() && !trip.priceMatrix[0].empty()) {
				// 对于行程记录，使用第一行最后一列的价格（总价格）
				price = trip.priceMatrix[0][trip.priceMatrix[0].size() - 1];
			}
			
			// 获取时间信息
			string departureTime = "";
			string arrivalTime = "";
			if (startIdx < trip.arrivalTimes.size()) {
				departureTime = trip.arrivalTimes[startIdx];
			}
			if (endIdx < trip.arrivalTimes.size()) {
				arrivalTime = trip.arrivalTimes[endIdx];
			}
			
			myTripsTable->insertRow(row);
			
			QTableWidgetItem* trainItem = new QTableWidgetItem(QString::fromStdString(trip.trainNumber));
			trainItem->setTextAlignment(Qt::AlignCenter);
			myTripsTable->setItem(row, 0, trainItem);
			
			QTableWidgetItem* startItem = new QTableWidgetItem(QString::fromStdString(startStation));
			startItem->setTextAlignment(Qt::AlignCenter);
			myTripsTable->setItem(row, 1, startItem);
			
			QTableWidgetItem* endItem = new QTableWidgetItem(QString::fromStdString(endStation));
			endItem->setTextAlignment(Qt::AlignCenter);
			myTripsTable->setItem(row, 2, endItem);
			
			QTableWidgetItem* depTimeItem = new QTableWidgetItem(QString::fromStdString(departureTime));
			depTimeItem->setTextAlignment(Qt::AlignCenter);
			myTripsTable->setItem(row, 3, depTimeItem);
			
			QTableWidgetItem* arrTimeItem = new QTableWidgetItem(QString::fromStdString(arrivalTime));
			arrTimeItem->setTextAlignment(Qt::AlignCenter);
			myTripsTable->setItem(row, 4, arrTimeItem);
			
			QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(price));
			priceItem->setTextAlignment(Qt::AlignCenter);
			myTripsTable->setItem(row, 5, priceItem);
			
			row++;
		}
	}
}

// 创建开始菜单界面
QWidget* createStartMenuWidget(QMainWindow* mainWindow) {
	QWidget* widget = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->setSpacing(30);
	layout->setContentsMargins(50, 50, 50, 50);
	
	// 标题
	QLabel* titleLabel = new QLabel("铁路票务管理系统");
	titleLabel->setAlignment(Qt::AlignCenter);
	titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; margin-bottom: 30px;");
	layout->addWidget(titleLabel);
	
	// 用户系统按钮
	QPushButton* userBtn = new QPushButton("用户系统");
	userBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; border: none; padding: 15px; font-size: 16px; border-radius: 5px; }"
						  "QPushButton:hover { background-color: #2980b9; }");
	layout->addWidget(userBtn);
	
	// 管理员系统按钮
	QPushButton* adminBtn = new QPushButton("管理员系统");
	adminBtn->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border: none; padding: 15px; font-size: 16px; border-radius: 5px; }"
						   "QPushButton:hover { background-color: #c0392b; }");
	layout->addWidget(adminBtn);
	
	// 按钮事件
	QObject::connect(userBtn, &QPushButton::clicked, [mainWindow]() {
		stackedWidget->setCurrentWidget(loginWidget);
	});
	
	QObject::connect(adminBtn, &QPushButton::clicked, [mainWindow]() {
		stackedWidget->setCurrentWidget(adminLoginWidget);
	});
	
	return widget;
}

// 创建登录界面
QWidget* createLoginWidget(QMainWindow* mainWindow) {
	QWidget* widget = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->setSpacing(20);
	layout->setContentsMargins(50, 50, 50, 50);
	
	// 标题
	QLabel* titleLabel = new QLabel("列车购票系统 - 登录");
	titleLabel->setAlignment(Qt::AlignCenter);
	titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #2c3e50; margin-bottom: 30px;");
	layout->addWidget(titleLabel);
	
	// 登录表单
	QWidget* formWidget = new QWidget();
	QFormLayout* formLayout = new QFormLayout(formWidget);
	formLayout->setLabelAlignment(Qt::AlignRight);
	formLayout->setFormAlignment(Qt::AlignHCenter);
	
	// 手机号输入
	QLineEdit* phoneEdit = new QLineEdit();
	phoneEdit->setPlaceholderText("请输入11位手机号码");
	phoneEdit->setMaxLength(11);
	phoneEdit->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; min-width: 200px; }");
	QLabel* phoneLabel = new QLabel("手机号:");
	phoneLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
	
	// 添加格式提示
	QLabel* phoneHintLabel = new QLabel("格式：11位数字");
	phoneHintLabel->setStyleSheet("font-size: 12px; color: #7f8c8d; margin-left: 5px;");
	
	QWidget* phoneWidget = new QWidget();
	QVBoxLayout* phoneLayout = new QVBoxLayout(phoneWidget);
	phoneLayout->setContentsMargins(0, 0, 0, 0);
	phoneLayout->addWidget(phoneEdit);
	phoneLayout->addWidget(phoneHintLabel);
	
	formLayout->addRow(phoneLabel, phoneWidget);
	
	// 密码输入
	QLineEdit* passwordEdit = new QLineEdit();
	passwordEdit->setEchoMode(QLineEdit::Password);
	passwordEdit->setPlaceholderText("请输入密码");
	passwordEdit->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; min-width: 200px; }");
	QLabel* passwordLabel = new QLabel("密码:");
	passwordLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
	
	// 添加密码格式提示
	QLabel* passwordHintLabel = new QLabel("格式：至少8位，包含大写字母、小写字母和数字");
	passwordHintLabel->setStyleSheet("font-size: 12px; color: #7f8c8d; margin-left: 5px;");
	
	QWidget* passwordWidget = new QWidget();
	QVBoxLayout* passwordLayout = new QVBoxLayout(passwordWidget);
	passwordLayout->setContentsMargins(0, 0, 0, 0);
	passwordLayout->addWidget(passwordEdit);
	passwordLayout->addWidget(passwordHintLabel);
	
	formLayout->addRow(passwordLabel, passwordWidget);
	
	layout->addWidget(formWidget);
	
	// 按钮区域
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	
	QPushButton* loginBtn = new QPushButton("登录");
	QPushButton* goToRegisterBtn = new QPushButton("注册新账户");
	QPushButton* exitBtn = new QPushButton("退出");
	
	QString loginButtonStyle = "QPushButton { font-size: 16px; padding: 12px 30px; margin: 5px; background-color: #3498db; color: white; border: none; border-radius: 5px; } QPushButton:hover { background-color: #2980b9; }";
	QString registerButtonStyle = "QPushButton { font-size: 16px; padding: 12px 30px; margin: 5px; background-color: #27ae60; color: white; border: none; border-radius: 5px; } QPushButton:hover { background-color: #229954; }";
	QString exitButtonStyle = "QPushButton { font-size: 16px; padding: 12px 30px; margin: 5px; background-color: #e74c3c; color: white; border: none; border-radius: 5px; } QPushButton:hover { background-color: #c0392b; }";
	
	loginBtn->setStyleSheet(loginButtonStyle);
	goToRegisterBtn->setStyleSheet(registerButtonStyle);
	exitBtn->setStyleSheet(exitButtonStyle);
	
	buttonLayout->addWidget(loginBtn);
	buttonLayout->addWidget(goToRegisterBtn);
	buttonLayout->addWidget(exitBtn);
	layout->addLayout(buttonLayout);
	
	// 登录按钮事件
	QObject::connect(loginBtn, &QPushButton::clicked, [mainWindow, phoneEdit, passwordEdit]() {
		QString phone = phoneEdit->text().trimmed();
		QString password = passwordEdit->text().trimmed();
		
		if (phone.isEmpty() || password.isEmpty()) {
			QMessageBox::warning(mainWindow, "错误", "请输入手机号和密码!");
			return;
		}
		
		// 验证手机号格式
		if (!isValidPhoneNumber(phone.toStdString())) {
			QMessageBox::warning(mainWindow, "格式错误", "手机号必须是11位数字!");
			return;
		}
		
		auto it = find_if(users.begin(), users.end(),
			[phone](const User& u) { return QString::fromStdString(u.phoneNumber) == phone; });
		
		if (it == users.end()) {
			QMessageBox::warning(mainWindow, "错误", "手机号不存在!");
			return;
		}
		
		if (QString::fromStdString(it->password) == password) {
			currentUser = &*it;
			stackedWidget->setCurrentWidget(mainMenuWidget);
			// 登录成功后立即更新余额显示和行程表
			updateBalanceDisplay();
			updateMyTripsTable();
		} else {
			QMessageBox::warning(mainWindow, "错误", "密码错误!");
		}
	});
	
	// 回车键登录
	QObject::connect(passwordEdit, &QLineEdit::returnPressed, loginBtn, &QPushButton::click);
	
	// 跳转到注册界面
	QObject::connect(goToRegisterBtn, &QPushButton::clicked, [mainWindow]() {
		stackedWidget->setCurrentWidget(registerWidget);
	});
	
	// 退出按钮事件
	QObject::connect(exitBtn, &QPushButton::clicked, []() {
		stackedWidget->setCurrentWidget(startMenuWidget);
	});
	
	return widget;
}

// 创建注册界面
QWidget* createRegisterWidget(QMainWindow* mainWindow) {
	QWidget* widget = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->setSpacing(20);
	layout->setContentsMargins(50, 50, 50, 50);
	
	// 标题
	QLabel* titleLabel = new QLabel("列车购票系统 - 注册");
	titleLabel->setAlignment(Qt::AlignCenter);
	titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #2c3e50; margin-bottom: 30px;");
	layout->addWidget(titleLabel);
	
	// 注册表单
	QWidget* formWidget = new QWidget();
	QFormLayout* formLayout = new QFormLayout(formWidget);
	formLayout->setLabelAlignment(Qt::AlignRight);
	formLayout->setFormAlignment(Qt::AlignHCenter);
	
	// 手机号输入
	QLineEdit* phoneEdit = new QLineEdit();
	phoneEdit->setPlaceholderText("请输入11位手机号码");
	phoneEdit->setMaxLength(11);
	phoneEdit->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; min-width: 200px; }");
	QLabel* phoneLabel = new QLabel("手机号:");
	phoneLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
	
	// 添加格式提示
	QLabel* phoneHintLabel = new QLabel("格式：11位数字");
	phoneHintLabel->setStyleSheet("font-size: 12px; color: #7f8c8d; margin-left: 5px;");
	
	QWidget* phoneWidget = new QWidget();
	QVBoxLayout* phoneLayout = new QVBoxLayout(phoneWidget);
	phoneLayout->setContentsMargins(0, 0, 0, 0);
	phoneLayout->addWidget(phoneEdit);
	phoneLayout->addWidget(phoneHintLabel);
	
	formLayout->addRow(phoneLabel, phoneWidget);
	
	// 密码输入
	QLineEdit* passwordEdit = new QLineEdit();
	passwordEdit->setEchoMode(QLineEdit::Password);
	passwordEdit->setPlaceholderText("请输入密码");
	passwordEdit->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; min-width: 200px; }");
	QLabel* passwordLabel = new QLabel("密码:");
	passwordLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
	
	// 添加密码格式提示
	QLabel* passwordHintLabel = new QLabel("格式：至少8位，包含大写字母、小写字母和数字");
	passwordHintLabel->setStyleSheet("font-size: 12px; color: #7f8c8d; margin-left: 5px;");
	
	QWidget* passwordWidget = new QWidget();
	QVBoxLayout* passwordLayout = new QVBoxLayout(passwordWidget);
	passwordLayout->setContentsMargins(0, 0, 0, 0);
	passwordLayout->addWidget(passwordEdit);
	passwordLayout->addWidget(passwordHintLabel);
	
	formLayout->addRow(passwordLabel, passwordWidget);
	
	// 确认密码输入
	QLineEdit* confirmPasswordEdit = new QLineEdit();
	confirmPasswordEdit->setEchoMode(QLineEdit::Password);
	confirmPasswordEdit->setPlaceholderText("请再次输入密码");
	confirmPasswordEdit->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; min-width: 200px; }");
	QLabel* confirmPasswordLabel = new QLabel("确认密码:");
	confirmPasswordLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
	formLayout->addRow(confirmPasswordLabel, confirmPasswordEdit);
	
	// 姓名输入
	QLineEdit* nameEdit = new QLineEdit();
	nameEdit->setPlaceholderText("请输入真实姓名");
	nameEdit->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; min-width: 200px; }");
	QLabel* nameLabel = new QLabel("姓名:");
	nameLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
	formLayout->addRow(nameLabel, nameEdit);
	
	// 身份证号输入
	QLineEdit* idNumberEdit = new QLineEdit();
	idNumberEdit->setPlaceholderText("请输入身份证号");
	idNumberEdit->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; min-width: 200px; }");
	QLabel* idNumberLabel = new QLabel("身份证号:");
	idNumberLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
	formLayout->addRow(idNumberLabel, idNumberEdit);
	
	layout->addWidget(formWidget);
	
	// 按钮区域
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	
	QPushButton* registerBtn = new QPushButton("注册");
	QPushButton* backToLoginBtn = new QPushButton("返回登录");
	
	QString registerButtonStyle = "QPushButton { font-size: 16px; padding: 12px 30px; margin: 5px; background-color: #27ae60; color: white; border: none; border-radius: 5px; } QPushButton:hover { background-color: #229954; }";
	QString backButtonStyle = "QPushButton { font-size: 16px; padding: 12px 30px; margin: 5px; background-color: #95a5a6; color: white; border: none; border-radius: 5px; } QPushButton:hover { background-color: #7f8c8d; }";
	
	registerBtn->setStyleSheet(registerButtonStyle);
	backToLoginBtn->setStyleSheet(backButtonStyle);
	
	buttonLayout->addWidget(registerBtn);
	buttonLayout->addWidget(backToLoginBtn);
	layout->addLayout(buttonLayout);
	
	// 注册按钮事件
	QObject::connect(registerBtn, &QPushButton::clicked, [mainWindow, phoneEdit, passwordEdit, confirmPasswordEdit, nameEdit, idNumberEdit]() {
		QString phone = phoneEdit->text().trimmed();
		QString password = passwordEdit->text().trimmed();
		QString confirmPassword = confirmPasswordEdit->text().trimmed();
		QString name = nameEdit->text().trimmed();
		QString idNumber = idNumberEdit->text().trimmed();
		
		if (phone.isEmpty() || password.isEmpty() || confirmPassword.isEmpty() || name.isEmpty() || idNumber.isEmpty()) {
			QMessageBox::warning(mainWindow, "错误", "请填写所有字段!");
			return;
		}
		
		// 验证手机号格式
		if (!isValidPhoneNumber(phone.toStdString())) {
			QMessageBox::warning(mainWindow, "格式错误", "手机号必须是11位数字!");
			return;
		}
		
		// 验证密码格式
		if (!isValidPassword(password.toStdString())) {
			QMessageBox::warning(mainWindow, "格式错误", "密码必须至少8位，且包含大写字母、小写字母和数字!");
			return;
		}
		
		if (password != confirmPassword) {
			QMessageBox::warning(mainWindow, "错误", "两次输入的密码不一致!");
			return;
		}
		
		// 检查手机号是否已存在
		for (const auto& user : users) {
			if (QString::fromStdString(user.phoneNumber) == phone) {
				QMessageBox::warning(mainWindow, "错误", "该手机号已注册!");
				return;
			}
		}
		
		// 简单的身份证号格式验证
		if (idNumber.length() != 18) {
			QMessageBox::warning(mainWindow, "错误", "身份证号格式不正确!");
			return;
		}
		
		users.emplace_back(phone.toStdString(), password.toStdString(), name.toStdString(), idNumber.toStdString(), 3000.0);
		saveUsersToDB();
		QMessageBox::information(mainWindow, "成功", "注册成功!请返回登录页面登录。");
		
		// 清空表单
		phoneEdit->clear();
		passwordEdit->clear();
		confirmPasswordEdit->clear();
		nameEdit->clear();
		idNumberEdit->clear();
		
		// 跳转到登录界面
		stackedWidget->setCurrentWidget(loginWidget);
	});
	
	// 返回登录按钮事件
	QObject::connect(backToLoginBtn, &QPushButton::clicked, [mainWindow]() {
		stackedWidget->setCurrentWidget(loginWidget);
	});
	
	return widget;
}

// 创建管理员登录界面
QWidget* createAdminLoginWidget(QMainWindow* mainWindow) {
	QWidget* widget = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->setSpacing(20);
	layout->setContentsMargins(50, 50, 50, 50);
	
	// 标题
	QLabel* titleLabel = new QLabel("管理员登录");
	titleLabel->setAlignment(Qt::AlignCenter);
	titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #e74c3c; margin-bottom: 30px;");
	layout->addWidget(titleLabel);
	
	// 表单布局
	QFormLayout* formLayout = new QFormLayout();
	formLayout->setSpacing(15);
	formLayout->setLabelAlignment(Qt::AlignRight);
	formLayout->setFormAlignment(Qt::AlignHCenter);
	
	// 用户名输入
	QLineEdit* usernameEdit = new QLineEdit();
	usernameEdit->setPlaceholderText("请输入管理员用户名");
	usernameEdit->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; min-width: 200px; }");
	QLabel* usernameLabel = new QLabel("用户名:");
	usernameLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
	formLayout->addRow(usernameLabel, usernameEdit);
	
	// 密码输入
	QLineEdit* passwordEdit = new QLineEdit();
	passwordEdit->setEchoMode(QLineEdit::Password);
	passwordEdit->setPlaceholderText("请输入密码");
	passwordEdit->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; min-width: 200px; }");
	QLabel* passwordLabel = new QLabel("密码:");
	passwordLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
	formLayout->addRow(passwordLabel, passwordEdit);
	
	layout->addLayout(formLayout);
	layout->addSpacing(20);
	
	// 按钮布局
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	
	QPushButton* loginBtn = new QPushButton("登录");
	loginBtn->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border: none; padding: 10px 20px; font-size: 14px; border-radius: 5px; }"
						   "QPushButton:hover { background-color: #c0392b; }");
	buttonLayout->addWidget(loginBtn);
	
	QPushButton* registerBtn = new QPushButton("注册");
	registerBtn->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; border: none; padding: 10px 20px; font-size: 14px; border-radius: 5px; }"
							  "QPushButton:hover { background-color: #7f8c8d; }");
	buttonLayout->addWidget(registerBtn);
	
	QPushButton* backBtn = new QPushButton("返回");
	backBtn->setStyleSheet("QPushButton { background-color: #34495e; color: white; border: none; padding: 10px 20px; font-size: 14px; border-radius: 5px; }"
						  "QPushButton:hover { background-color: #2c3e50; }");
	buttonLayout->addWidget(backBtn);
	
	layout->addLayout(buttonLayout);
	
	// 登录按钮事件
	QObject::connect(loginBtn, &QPushButton::clicked, [mainWindow, usernameEdit, passwordEdit]() {
		QString username = usernameEdit->text();
		QString password = passwordEdit->text();
		
		if (username.isEmpty() || password.isEmpty()) {
			QMessageBox::warning(mainWindow, "错误", "请填写用户名和密码!");
			return;
		}
		
		// 验证管理员身份
		for (auto& admin : admins) {
			if (QString::fromStdString(admin.username) == username && QString::fromStdString(admin.password) == password) {
				currentAdmin = &admin;
				stackedWidget->setCurrentWidget(adminMenuWidget);
				usernameEdit->clear();
				passwordEdit->clear();
				return;
			}
		}
		
		QMessageBox::warning(mainWindow, "错误", "用户名或密码错误!");
	});
	
	// 注册按钮事件
	QObject::connect(registerBtn, &QPushButton::clicked, [mainWindow]() {
		stackedWidget->setCurrentWidget(adminRegisterWidget);
	});
	
	// 返回按钮事件
	QObject::connect(backBtn, &QPushButton::clicked, [mainWindow]() {
		stackedWidget->setCurrentWidget(startMenuWidget);
	});
	
	return widget;
}

// 创建管理员注册界面
QWidget* createAdminRegisterWidget(QMainWindow* mainWindow) {
	QWidget* widget = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->setSpacing(20);
	layout->setContentsMargins(50, 50, 50, 50);
	
	// 标题
	QLabel* titleLabel = new QLabel("管理员注册");
	titleLabel->setAlignment(Qt::AlignCenter);
	titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #e74c3c; margin-bottom: 30px;");
	layout->addWidget(titleLabel);
	
	// 表单布局
	QFormLayout* formLayout = new QFormLayout();
	formLayout->setSpacing(15);
	formLayout->setLabelAlignment(Qt::AlignRight);
	formLayout->setFormAlignment(Qt::AlignHCenter);
	
	// 用户名输入
	QLineEdit* usernameEdit = new QLineEdit();
	usernameEdit->setPlaceholderText("请输入管理员用户名");
	usernameEdit->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; min-width: 200px; }");
	QLabel* usernameLabel = new QLabel("用户名:");
	usernameLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
	formLayout->addRow(usernameLabel, usernameEdit);
	
	// 密码输入
	QLineEdit* passwordEdit = new QLineEdit();
	passwordEdit->setEchoMode(QLineEdit::Password);
	passwordEdit->setPlaceholderText("请输入密码");
	passwordEdit->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; min-width: 200px; }");
	QLabel* passwordLabel = new QLabel("密码:");
	passwordLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
	formLayout->addRow(passwordLabel, passwordEdit);
	
	// 确认密码输入
	QLineEdit* confirmPasswordEdit = new QLineEdit();
	confirmPasswordEdit->setEchoMode(QLineEdit::Password);
	confirmPasswordEdit->setPlaceholderText("请再次输入密码");
	confirmPasswordEdit->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; min-width: 200px; }");
	QLabel* confirmPasswordLabel = new QLabel("确认密码:");
	confirmPasswordLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
	formLayout->addRow(confirmPasswordLabel, confirmPasswordEdit);
	
	// 姓名输入
	QLineEdit* nameEdit = new QLineEdit();
	nameEdit->setPlaceholderText("请输入姓名");
	nameEdit->setStyleSheet("QLineEdit { font-size: 14px; padding: 8px; border: 2px solid #bdc3c7; border-radius: 5px; min-width: 200px; }");
	QLabel* nameLabel = new QLabel("姓名:");
	nameLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
	formLayout->addRow(nameLabel, nameEdit);
	
	layout->addLayout(formLayout);
	layout->addSpacing(20);
	
	// 按钮布局
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	
	QPushButton* registerBtn = new QPushButton("注册");
	registerBtn->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border: none; padding: 10px 20px; font-size: 14px; border-radius: 5px; }"
							  "QPushButton:hover { background-color: #c0392b; }");
	buttonLayout->addWidget(registerBtn);
	
	QPushButton* backToLoginBtn = new QPushButton("返回登录");
	backToLoginBtn->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; border: none; padding: 10px 20px; font-size: 14px; border-radius: 5px; }"
								 "QPushButton:hover { background-color: #7f8c8d; }");
	buttonLayout->addWidget(backToLoginBtn);
	
	layout->addLayout(buttonLayout);
	
	// 注册按钮事件
	QObject::connect(registerBtn, &QPushButton::clicked, [mainWindow, usernameEdit, passwordEdit, confirmPasswordEdit, nameEdit]() {
		QString username = usernameEdit->text();
		QString password = passwordEdit->text();
		QString confirmPassword = confirmPasswordEdit->text();
		QString name = nameEdit->text();
		
		if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty() || name.isEmpty()) {
			QMessageBox::warning(mainWindow, "错误", "请填写所有字段!");
			return;
		}
		
		if (password != confirmPassword) {
			QMessageBox::warning(mainWindow, "错误", "两次输入的密码不一致!");
			return;
		}
		
		// 检查用户名是否已存在
		for (const auto& admin : admins) {
			if (QString::fromStdString(admin.username) == username) {
				QMessageBox::warning(mainWindow, "错误", "管理员用户名已存在!");
				return;
			}
		}
		
		// 创建新管理员
		admins.emplace_back(username.toStdString(), password.toStdString(), name.toStdString());
		saveAdminsToDB();
		
		QMessageBox::information(mainWindow, "成功", "管理员注册成功!");
		
		// 清空输入框
		usernameEdit->clear();
		passwordEdit->clear();
		confirmPasswordEdit->clear();
		nameEdit->clear();
		
		// 跳转到登录界面
		stackedWidget->setCurrentWidget(adminLoginWidget);
	});
	
	// 返回登录按钮事件
	QObject::connect(backToLoginBtn, &QPushButton::clicked, [mainWindow]() {
		stackedWidget->setCurrentWidget(adminLoginWidget);
	});
	
	return widget;
}

// 更新车票搜索结果
void updateTicketTable(const QString& startStation, const QString& endStation, const QString& departureTimeFilter = "") {
	cout << "开始查票：从 " << startStation.toStdString() << " 到 " << endStation.toStdString() << endl;
	cout << "当前加载的车次数量: " << trains.size() << endl;
	
	if (!ticketTable) return;
	
	ticketTable->setRowCount(0);
	
	string start = startStation.toStdString();
	string end = endStation.toStdString();
	
	// 用于存储搜索结果和票价的结构体
	struct TicketResult {
		string trainNumber;
		string startStationName;
		string endStationName;
		string departureTime;
		string arrivalTime;
		int availableSeats;
		int price;
	};
	
	vector<TicketResult> results;
	
	for (const auto& train : trains) {
		// 检查列车是否被停开
		if (find(suspendedTrains.begin(), suspendedTrains.end(), train.trainNumber) != suspendedTrains.end()) {
			cout << "车次 " << train.trainNumber << " 已停开，跳过" << endl;
			continue;
		}
		
		cout << "检查车次: " << train.trainNumber << endl;
		cout << "  车次站点: ";
		for (const string& station : train.stations) {
			cout << station << " ";
		}
		cout << endl;
		auto startIt = find_if(train.stations.begin(), train.stations.end(),
			[&start](const string& station) {
				return station == start;
			});
		auto endIt = find_if(train.stations.begin(), train.stations.end(),
			[&end](const string& station) {
				return station == end;
			});
		
		bool foundStart = (startIt != train.stations.end());
		bool foundEnd = (endIt != train.stations.end());
		cout << "  找到起点站: " << (foundStart ? "是" : "否") << endl;
		cout << "  找到终点站: " << (foundEnd ? "是" : "否") << endl;
		
		if (startIt != train.stations.end() && endIt != train.stations.end() && startIt != endIt) {
			size_t startIdx = distance(train.stations.begin(), startIt);
			size_t endIdx = distance(train.stations.begin(), endIt);
			
			// 确保索引顺序正确（小的在前，大的在后）
			size_t fromIdx = min(startIdx, endIdx);
			size_t toIdx = max(startIdx, endIdx);
			
			// 安全检查数组边界 - 现在检查站点索引而不是时间索引
			size_t numStations = train.stations.size();
			if (fromIdx < numStations && toIdx < numStations &&
				fromIdx < train.segmentAvailableSeats.size() && 
				fromIdx < train.priceMatrix.size() &&
				toIdx < train.segmentAvailableSeats[fromIdx].size() &&
				toIdx < train.priceMatrix[fromIdx].size()) {
				
				// 根据实际出发和到达站点确定时间和其他信息
				// 获取对应方向的时刻表
				vector<string> scheduleTimes = getDirectionalSchedule(train.arrivalTimes, startIdx, endIdx);
				string departureTime = scheduleTimes[startIdx];
				string arrivalTime = scheduleTimes[endIdx];
				
				// 如果用户指定了出发时间，进行过滤
				if (!departureTimeFilter.isEmpty()) {
					int filterTime = timeToMinutes(departureTimeFilter.toStdString());
					int trainDepartureTime = timeToMinutes(departureTime);
					
					// 如果列车出发时间早于用户指定的时间，跳过此车次
					if (trainDepartureTime < filterTime) {
						continue;
					}
				}
				
				int availableSeats = train.segmentAvailableSeats[fromIdx][toIdx];
				int price = train.priceMatrix[fromIdx][toIdx];
				
				results.push_back({
					train.trainNumber,
					train.stations[startIdx],
					train.stations[endIdx],
					departureTime,
					arrivalTime,
					availableSeats,
					price
				});
			}
		}
	}
	
	// 如果没有找到结果，显示消息
	if (results.empty()) {
		QMessageBox::information(nullptr, "查询结果", 
			QString("未找到从 %1 到 %2 的车票\n\n提示：请检查站点名称是否正确\n例如：北京, 上海, 广州")
			.arg(startStation).arg(endStation));
		return;
	}
	
	// 按票价从低到高排序
	sort(results.begin(), results.end(), [](const TicketResult& a, const TicketResult& b) {
		return a.price < b.price;
	});
	
	// 填充表格
	int row = 0;
	for (const auto& result : results) {
		ticketTable->insertRow(row);
		
		QTableWidgetItem* trainItem = new QTableWidgetItem(QString::fromStdString(result.trainNumber));
		trainItem->setTextAlignment(Qt::AlignCenter);
		ticketTable->setItem(row, 0, trainItem);
		
		QTableWidgetItem* startItem = new QTableWidgetItem(QString::fromStdString(result.startStationName));
		startItem->setTextAlignment(Qt::AlignCenter);
		ticketTable->setItem(row, 1, startItem);
		
		QTableWidgetItem* endItem = new QTableWidgetItem(QString::fromStdString(result.endStationName));
		endItem->setTextAlignment(Qt::AlignCenter);
		ticketTable->setItem(row, 2, endItem);
		
		QTableWidgetItem* depTimeItem = new QTableWidgetItem(QString::fromStdString(result.departureTime));
		depTimeItem->setTextAlignment(Qt::AlignCenter);
		ticketTable->setItem(row, 3, depTimeItem);
		
		QTableWidgetItem* arrTimeItem = new QTableWidgetItem(QString::fromStdString(result.arrivalTime));
		arrTimeItem->setTextAlignment(Qt::AlignCenter);
		ticketTable->setItem(row, 4, arrTimeItem);
		
		QTableWidgetItem* seatsItem = new QTableWidgetItem(QString::number(result.availableSeats));
		seatsItem->setTextAlignment(Qt::AlignCenter);
		ticketTable->setItem(row, 5, seatsItem);
		
		QTableWidgetItem* priceItem = new QTableWidgetItem(QString::number(result.price));
		priceItem->setTextAlignment(Qt::AlignCenter);
		ticketTable->setItem(row, 6, priceItem);
		
		row++;
	}
}

// 创建主菜单界面
QWidget* createMainMenuWidget(QMainWindow* mainWindow) {
	QWidget* widget = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout(widget);
	
	// 欢迎标题
	QLabel* welcomeLabel = new QLabel("欢迎使用列车购票系统");
	welcomeLabel->setAlignment(Qt::AlignCenter);
	welcomeLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin: 10px;");
	layout->addWidget(welcomeLabel);
	
	// 创建标签页
	QTabWidget* tabWidget = new QTabWidget();
	
	// 车票查询标签页
	QWidget* searchTab = new QWidget();
	QVBoxLayout* searchLayout = new QVBoxLayout(searchTab);
	
	// 查询输入区域
	QGroupBox* searchGroup = new QGroupBox("车票查询");
	searchGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 14px; margin: 10px 0px; padding: 10px; border: 2px solid #bdc3c7; border-radius: 5px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");
	QHBoxLayout* searchInputLayout = new QHBoxLayout(searchGroup);
	
	QLabel* fromLabel = new QLabel("出发站:");
	fromLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
	QLineEdit* fromEdit = new QLineEdit();
	fromEdit->setPlaceholderText("例如: 北京");
	fromEdit->setStyleSheet("QLineEdit { padding: 8px; border: 2px solid #bdc3c7; border-radius: 3px; font-size: 14px; min-width: 120px; }");
	
	QLabel* toLabel = new QLabel("到达站:");
	toLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
	QLineEdit* toEdit = new QLineEdit();
	toEdit->setPlaceholderText("例如: 上海");
	toEdit->setStyleSheet("QLineEdit { padding: 8px; border: 2px solid #bdc3c7; border-radius: 3px; font-size: 14px; min-width: 120px; }");
	
	QLabel* timeLabel = new QLabel("出发时间:");
	timeLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
	
	// 时间输入区域
	QWidget* timeWidget = new QWidget();
	QHBoxLayout* timeLayout = new QHBoxLayout(timeWidget);
	timeLayout->setContentsMargins(0, 0, 0, 0);
	timeLayout->setSpacing(5);
	
	QLineEdit* hourEdit = new QLineEdit();
	hourEdit->setPlaceholderText("时");
	hourEdit->setMaxLength(2);
	hourEdit->setFixedWidth(40);
	hourEdit->setStyleSheet("QLineEdit { padding: 8px; border: 2px solid #bdc3c7; border-radius: 3px; font-size: 14px; text-align: center; }");
	
	// 添加输入验证器，只允许输入0-23的数字
	QIntValidator* hourValidator = new QIntValidator(0, 23, hourEdit);
	hourEdit->setValidator(hourValidator);
	
	QLabel* colonLabel = new QLabel(":");
	colonLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50;");
	
	QLineEdit* minuteEdit = new QLineEdit();
	minuteEdit->setPlaceholderText("分");
	minuteEdit->setMaxLength(2);
	minuteEdit->setFixedWidth(40);
	minuteEdit->setStyleSheet("QLineEdit { padding: 8px; border: 2px solid #bdc3c7; border-radius: 3px; font-size: 14px; text-align: center; }");
	
	// 添加输入验证器，只允许输入0-59的数字
	QIntValidator* minuteValidator = new QIntValidator(0, 59, minuteEdit);
	minuteEdit->setValidator(minuteValidator);
	
	timeLayout->addWidget(hourEdit);
	timeLayout->addWidget(colonLabel);
	timeLayout->addWidget(minuteEdit);
	timeLayout->addStretch();
	
	QPushButton* searchBtn = new QPushButton("查询车票");
	searchBtn->setStyleSheet("QPushButton { background-color: #27ae60; color: white; padding: 10px 20px; border: none; border-radius: 5px; font-size: 14px; font-weight: bold; } QPushButton:hover { background-color: #229954; }");
	
	searchInputLayout->addWidget(fromLabel);
	searchInputLayout->addWidget(fromEdit);
	searchInputLayout->addStretch();
	searchInputLayout->addWidget(toLabel);
	searchInputLayout->addWidget(toEdit);
	searchInputLayout->addStretch();
	searchInputLayout->addWidget(timeLabel);
	searchInputLayout->addWidget(timeWidget);
	searchInputLayout->addStretch();
	searchInputLayout->addWidget(searchBtn);
	searchLayout->addWidget(searchGroup);
	
	// 车票结果表格
	ticketTable = new QTableWidget();
	ticketTable->setColumnCount(7);
	QStringList headers = {"车次", "出发站", "到达站", "出发时间", "到达时间", "余票", "票价(¥)"};
	ticketTable->setHorizontalHeaderLabels(headers);
	ticketTable->horizontalHeader()->setStretchLastSection(true);
	ticketTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	ticketTable->setSelectionMode(QAbstractItemView::SingleSelection);
	ticketTable->setAlternatingRowColors(true);
	ticketTable->setStyleSheet("QTableWidget { border: 1px solid #bdc3c7; gridline-color: #ecf0f1; background-color: #ffffff; } QTableWidget::item { padding: 8px; text-align: center; } QHeaderView::section { background-color: #34495e; color: white; padding: 8px; font-weight: bold; text-align: center; }");
	searchLayout->addWidget(ticketTable);
	
	// 购票按钮
	QPushButton* buyBtn = new QPushButton("购买选中车票");
	buyBtn->setStyleSheet("QPushButton { font-size: 14px; padding: 10px 20px; margin: 5px; background-color: #3498db; color: white; border: none; border-radius: 5px; font-weight: bold; } QPushButton:hover { background-color: #2980b9; }");
	searchLayout->addWidget(buyBtn);
	
	tabWidget->addTab(searchTab, "车票查询");
	
	// 个人行程标签页
	QWidget* tripsTab = new QWidget();
	QVBoxLayout* tripsLayout = new QVBoxLayout(tripsTab);
	
	// 乘车人信息显示区域
	QGroupBox* tripsPassengerGroup = new QGroupBox("乘车人信息");
	tripsPassengerGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 14px; margin: 10px 0px; padding: 10px; border: 2px solid #bdc3c7; border-radius: 5px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");
	QVBoxLayout* tripsPassengerLayout = new QVBoxLayout(tripsPassengerGroup);
	
	// 乘车人姓名显示
	QHBoxLayout* passengerNameLayout = new QHBoxLayout();
	tripsPassengerNameLabel = new QLabel("乘车人：未登录");
	tripsPassengerNameLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; padding: 10px;");
	passengerNameLayout->addWidget(tripsPassengerNameLabel);
	passengerNameLayout->addStretch();
	tripsPassengerLayout->addLayout(passengerNameLayout);
	
	// 手机号显示
	QHBoxLayout* passengerPhoneLayout = new QHBoxLayout();
	tripsPassengerPhoneLabel = new QLabel("手机号：未登录");
	tripsPassengerPhoneLabel->setStyleSheet("font-size: 16px; color: #7f8c8d; padding: 5px 10px;");
	passengerPhoneLayout->addWidget(tripsPassengerPhoneLabel);
	passengerPhoneLayout->addStretch();
	tripsPassengerLayout->addLayout(passengerPhoneLayout);
	
	tripsLayout->addWidget(tripsPassengerGroup);
	
	// 行程信息区域
	QGroupBox* tripsGroup = new QGroupBox("我的行程");
	tripsGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 14px; margin: 10px 0px; padding: 10px; border: 2px solid #bdc3c7; border-radius: 5px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");
	QVBoxLayout* tripsGroupLayout = new QVBoxLayout(tripsGroup);
	
	// 个人行程表格
	myTripsTable = new QTableWidget();
	myTripsTable->setColumnCount(6);
	QStringList tripHeaders = {"车次", "出发站", "到达站", "出发时间", "到达时间", "票价(¥)"};
	myTripsTable->setHorizontalHeaderLabels(tripHeaders);
	myTripsTable->horizontalHeader()->setStretchLastSection(true);
	myTripsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	myTripsTable->setSelectionMode(QAbstractItemView::SingleSelection);
	myTripsTable->setAlternatingRowColors(true);
	myTripsTable->setStyleSheet("QTableWidget { border: 1px solid #bdc3c7; gridline-color: #ecf0f1; background-color: #ffffff; } QTableWidget::item { padding: 8px; text-align: center; } QHeaderView::section { background-color: #34495e; color: white; padding: 8px; font-weight: bold; text-align: center; }");
	tripsGroupLayout->addWidget(myTripsTable);
	
	// 退票按钮
	QPushButton* cancelBtn = new QPushButton("退订选中车票");
	cancelBtn->setStyleSheet("QPushButton { font-size: 14px; padding: 10px 20px; margin: 5px; background-color: #e74c3c; color: white; border: none; border-radius: 5px; font-weight: bold; } QPushButton:hover { background-color: #c0392b; }");
	tripsGroupLayout->addWidget(cancelBtn);
	
	tripsLayout->addWidget(tripsGroup);
	
	tabWidget->addTab(tripsTab, "我的行程");
	
	// 账户管理标签页
	QWidget* accountTab = new QWidget();
	QVBoxLayout* accountLayout = new QVBoxLayout(accountTab);
	
	// 余额显示和充值功能
	QGroupBox* balanceGroup = new QGroupBox("账户信息");
	balanceGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 14px; margin: 10px 0px; padding: 10px; border: 2px solid #bdc3c7; border-radius: 5px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");
	QVBoxLayout* balanceGroupLayout = new QVBoxLayout(balanceGroup);
	
	// 乘车人信息显示
	QHBoxLayout* passengerInfoLayout = new QHBoxLayout();
	passengerInfoLabel = new QLabel("乘车人：未登录");
	passengerInfoLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; padding: 15px; text-align: center;");
	passengerInfoLayout->addWidget(passengerInfoLabel);
	passengerInfoLayout->addStretch();
	balanceGroupLayout->addLayout(passengerInfoLayout);
	
	// 余额显示行
	QHBoxLayout* balanceDisplayLayout = new QHBoxLayout();
	balanceLabel = new QLabel("当前余额: ¥0.00");
	balanceLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #27ae60; padding: 20px; text-align: center;");
	balanceDisplayLayout->addWidget(balanceLabel);
	balanceDisplayLayout->addStretch();
	balanceGroupLayout->addLayout(balanceDisplayLayout);
	
	// 充值操作区域
	QGroupBox* rechargeGroup = new QGroupBox("账户充值");
	rechargeGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 14px; margin: 10px 0px; padding: 10px; border: 2px solid #bdc3c7; border-radius: 5px; } QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");
	QVBoxLayout* rechargeGroupLayout = new QVBoxLayout(rechargeGroup);
	
	QHBoxLayout* rechargeLayout = new QHBoxLayout();
	QLabel* rechargeLabel = new QLabel("充值金额:");
	rechargeLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
	rechargeAmountEdit = new QLineEdit();
	rechargeAmountEdit->setPlaceholderText("输入充值金额");
	rechargeAmountEdit->setMaximumWidth(200);
	rechargeAmountEdit->setStyleSheet("QLineEdit { padding: 12px; border: 2px solid #bdc3c7; border-radius: 5px; font-size: 16px; }");
	QPushButton* rechargeBtn = new QPushButton("立即充值");
	rechargeBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; padding: 12px 25px; border: none; border-radius: 5px; font-size: 16px; font-weight: bold; } QPushButton:hover { background-color: #2980b9; }");
	
	rechargeLayout->addWidget(rechargeLabel);
	rechargeLayout->addWidget(rechargeAmountEdit);
	rechargeLayout->addWidget(rechargeBtn);
	rechargeLayout->addStretch();
	rechargeGroupLayout->addLayout(rechargeLayout);
	
	balanceGroupLayout->addWidget(rechargeGroup);
	accountLayout->addWidget(balanceGroup);
	accountLayout->addStretch();
	
	tabWidget->addTab(accountTab, "账户管理");
	
	layout->addWidget(tabWidget);
	
	// 退出登录按钮
	QPushButton* logoutBtn = new QPushButton("退出登录");
	logoutBtn->setStyleSheet("QPushButton { font-size: 14px; padding: 8px; margin: 5px; }");
	layout->addWidget(logoutBtn);
	
	// 查询按钮事件
	QObject::connect(searchBtn, &QPushButton::clicked, [fromEdit, toEdit, hourEdit, minuteEdit]() {
		QString startStation = fromEdit->text().trimmed();
		QString endStation = toEdit->text().trimmed();
		QString hourText = hourEdit->text().trimmed();
		QString minuteText = minuteEdit->text().trimmed();
		
		if (startStation.isEmpty() || endStation.isEmpty()) {
			QMessageBox::warning(nullptr, "提示", "请输入出发站和到达站!");
			return;
		}
		
		QString departureTime = "";
		
		// 验证时间输入（如果用户输入了时间）
		if (!hourText.isEmpty() || !minuteText.isEmpty()) {
			// 如果只输入了其中一个，提示用户
			if (hourText.isEmpty() || minuteText.isEmpty()) {
				QMessageBox::warning(nullptr, "提示", "请输入完整的时间（小时和分钟）或留空不限制时间");
				return;
			}
			
			bool hourOk, minuteOk;
			int hour = hourText.toInt(&hourOk);
			int minute = minuteText.toInt(&minuteOk);
			
			if (!hourOk || !minuteOk || hour < 0 || hour > 23 || minute < 0 || minute > 59) {
				QMessageBox::warning(nullptr, "提示", "请输入有效的时间\n小时：0-23\n分钟：0-59");
				return;
			}
			
			// 格式化时间为HH:MM
			departureTime = QString("%1:%2")
				.arg(hour, 2, 10, QChar('0'))
				.arg(minute, 2, 10, QChar('0'));
		}
		
		currentStartStation = startStation.toStdString();
		currentEndStation = endStation.toStdString();
		currentDepartureTimeFilter = departureTime.toStdString();
		updateTicketTable(startStation, endStation, departureTime);
	});
	
	// 购票按钮事件
	QObject::connect(buyBtn, &QPushButton::clicked, [mainWindow]() {
		int currentRow = ticketTable->currentRow();
		if (currentRow < 0) {
			QMessageBox::warning(mainWindow, "提示", "请选择要购买的车票!");
			return;
		}
		
		if (currentStartStation.empty() || currentEndStation.empty()) {
			QMessageBox::warning(mainWindow, "提示", "请先查询车票!");
			return;
		}
		
		if (!currentUser) {
			QMessageBox::warning(mainWindow, "错误", "用户未登录!");
			return;
		}
		
		// 检查表格项是否存在
		QTableWidgetItem* item = ticketTable->item(currentRow, 0);
		if (!item) {
			QMessageBox::warning(mainWindow, "错误", "获取车次信息失败!");
			return;
		}
		
		QString trainNumber = item->text();
		
		// 查找列车并购票
		auto trainIt = find_if(trains.begin(), trains.end(),
			[trainNumber](const Train& t) { return QString::fromStdString(t.trainNumber) == trainNumber; });
		
		if (trainIt != trains.end()) {
			string start = currentStartStation;
			string end = currentEndStation;
			
			auto startIt = find_if(trainIt->stations.begin(), trainIt->stations.end(),
				[&start](const string& station) { return station == start; });
			auto endIt = find_if(trainIt->stations.begin(), trainIt->stations.end(),
				[&end](const string& station) { return station == end; });
			
			if (startIt != trainIt->stations.end() && endIt != trainIt->stations.end() && startIt != endIt) {
				size_t startIdx = distance(trainIt->stations.begin(), startIt);
				size_t endIdx = distance(trainIt->stations.begin(), endIt);
				
				// 确保索引顺序正确（小的在前，大的在后）
				size_t fromIdx = min(startIdx, endIdx);
				size_t toIdx = max(startIdx, endIdx);
				
				// 安全检查数组边界
				if (fromIdx < trainIt->segmentAvailableSeats.size() && 
					fromIdx < trainIt->priceMatrix.size() &&
					toIdx < trainIt->segmentAvailableSeats[fromIdx].size() &&
					toIdx < trainIt->priceMatrix[fromIdx].size() &&
					trainIt->segmentAvailableSeats[fromIdx][toIdx] > 0) {
					
					int ticketPrice = trainIt->priceMatrix[fromIdx][toIdx];
					
					// 检查余额是否足够
					if (currentUser->balance < ticketPrice) {
						QMessageBox::warning(mainWindow, "余额不足", 
							QString("购票失败！\n票价: ¥%1\n当前余额: ¥%2\n需要充值: ¥%3")
							.arg(ticketPrice)
							.arg(QString::number(currentUser->balance, 'f', 2))
							.arg(QString::number(ticketPrice - currentUser->balance, 'f', 2)));
						return;
					}
					
					// 扣除余额
					currentUser->balance -= ticketPrice;
					saveUsersToDB();
					updateBalanceDisplay();
					
					// 创建行程记录（只保存起点和终点）
					vector<string> tripStations;
					vector<string> tripArrivalTimes;
					
					// 只保存起点和终点站
					tripStations.push_back(trainIt->stations[startIdx]);
					tripStations.push_back(trainIt->stations[endIdx]);
					
					// 为行程生成适当的时刻表
					vector<string> fullSchedule = getDirectionalSchedule(trainIt->arrivalTimes, startIdx, endIdx);
					tripArrivalTimes.push_back(fullSchedule[startIdx]);
					tripArrivalTimes.push_back(fullSchedule[endIdx]);
					
					// 创建简化的价格矩阵（只包含起点到终点的价格）
					vector<vector<int>> tripSegmentAvailableSeats(2, vector<int>(2, 0));
					vector<vector<int>> tripPriceMatrix(2, vector<int>(2, 0));
					
					// 设置从起点到终点的价格
					size_t fromIdx = min(startIdx, endIdx);
					size_t toIdx = max(startIdx, endIdx);
					tripPriceMatrix[0][1] = trainIt->priceMatrix[fromIdx][toIdx];
					tripSegmentAvailableSeats[0][1] = trainIt->segmentAvailableSeats[fromIdx][toIdx];
					
					Train tripRecord(trainNumber.toStdString(), tripStations, tripArrivalTimes, tripSegmentAvailableSeats, tripPriceMatrix);
					currentUser->trips.push_back(tripRecord);
					
					// 保存用户数据（包括新购买的车票）
					saveUsersToDB();
					
					// 更新余票
					trainIt->segmentAvailableSeats[fromIdx][toIdx]--;
					saveTrainsToDB();
					
					QMessageBox::information(mainWindow, "购票成功", 
						QString("购票成功！\n车次: %1\n从 %2 到 %3\n票价: ¥%4\n剩余余额: ¥%5")
						.arg(trainNumber)
						.arg(QString::fromStdString(currentStartStation))
						.arg(QString::fromStdString(currentEndStation))
						.arg(ticketPrice)
						.arg(QString::number(currentUser->balance, 'f', 2)));
					
					// 刷新表格
					updateTicketTable(QString::fromStdString(currentStartStation), QString::fromStdString(currentEndStation), QString::fromStdString(currentDepartureTimeFilter));
					updateMyTripsTable();
				} else {
					QMessageBox::warning(mainWindow, "错误", "该区间已无余票或数据错误!");
				}
			} else {
				QMessageBox::warning(mainWindow, "错误", "站点信息错误!");
			}
		} else {
			QMessageBox::warning(mainWindow, "错误", "未找到该车次!");
		}
	});
	
	// 退票按钮事件
	QObject::connect(cancelBtn, &QPushButton::clicked, [mainWindow]() {
		int currentRow = myTripsTable->currentRow();
		if (currentRow < 0) {
			QMessageBox::warning(mainWindow, "提示", "请选择要退订的车票!");
			return;
		}
		
		QString trainNumber = myTripsTable->item(currentRow, 0)->text();
		
		// 查找并删除行程
		auto tripIt = find_if(currentUser->trips.begin(), currentUser->trips.end(),
			[trainNumber](const Train& t) { return QString::fromStdString(t.trainNumber) == trainNumber; });
		
		if (tripIt != currentUser->trips.end()) {
			// 获取行程的起点和终点站
			string startStation = tripIt->stations.front();
			string endStation = tripIt->stations.back();
			
			// 计算退款金额 - 只返还80%的票价
			int originalPrice = 0;
			if (!tripIt->priceMatrix.empty() && !tripIt->priceMatrix[0].empty()) {
				originalPrice = tripIt->priceMatrix[0][tripIt->priceMatrix[0].size() - 1];
			}
			int refundAmount = static_cast<int>(originalPrice * 0.8); // 80%退款
			
			// 恢复余票
			auto trainIt = find_if(trains.begin(), trains.end(),
				[trainNumber](const Train& t) { return QString::fromStdString(t.trainNumber) == trainNumber; });
			
			if (trainIt != trains.end()) {
				auto startIt = find_if(trainIt->stations.begin(), trainIt->stations.end(),
					[&startStation](const string& station) { return station == startStation; });
				auto endIt = find_if(trainIt->stations.begin(), trainIt->stations.end(),
					[&endStation](const string& station) { return station == endStation; });
				
				if (startIt != trainIt->stations.end() && endIt != trainIt->stations.end()) {
					size_t startIdx = distance(trainIt->stations.begin(), startIt);
					size_t endIdx = distance(trainIt->stations.begin(), endIt);
					size_t fromIdx = min(startIdx, endIdx);
					size_t toIdx = max(startIdx, endIdx);
					trainIt->segmentAvailableSeats[fromIdx][toIdx]++;
				}
			}
			
			// 退款到余额
			currentUser->balance += refundAmount;
			saveUsersToDB();
			updateBalanceDisplay();
			
			currentUser->trips.erase(tripIt);
			saveTrainsToDB();
			
			QMessageBox::information(mainWindow, "退票成功", 
				QString("退票成功！\n原票价: ¥%1\n退款金额: ¥%2 (80%)\n当前余额: ¥%3")
				.arg(originalPrice)
				.arg(refundAmount)
				.arg(QString::number(currentUser->balance, 'f', 2)));
			updateMyTripsTable();
		}
	});
	
	// 退出登录按钮事件
	QObject::connect(logoutBtn, &QPushButton::clicked, []() {
		currentUser = nullptr;
		stackedWidget->setCurrentWidget(loginWidget);
	});
	
	// 充值按钮事件
	QObject::connect(rechargeBtn, &QPushButton::clicked, [mainWindow]() {
		if (!currentUser) {
			QMessageBox::warning(mainWindow, "错误", "用户未登录!");
			return;
		}
		
		QString amountText = rechargeAmountEdit->text().trimmed();
		if (amountText.isEmpty()) {
			QMessageBox::warning(mainWindow, "提示", "请输入充值金额!");
			return;
		}
		
		bool ok;
		double amount = amountText.toDouble(&ok);
		if (!ok || amount <= 0) {
			QMessageBox::warning(mainWindow, "错误", "请输入有效的充值金额!");
			return;
		}
		
		if (amount > 10000) {
			QMessageBox::warning(mainWindow, "错误", "单次充值金额不能超过¥10000!");
			return;
		}
		
		currentUser->balance += amount;
		saveUsersToDB();
		updateBalanceDisplay();
		rechargeAmountEdit->clear();
		
		QMessageBox::information(mainWindow, "充值成功", 
			QString("充值成功！\n充值金额: ¥%1\n当前余额: ¥%2")
			.arg(QString::number(amount, 'f', 2))
			.arg(QString::number(currentUser->balance, 'f', 2)));
	});
	
	return widget;
}

// 创建管理员主菜单界面
QWidget* createAdminMenuWidget(QMainWindow* mainWindow) {
	QWidget* widget = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->setSpacing(20);
	layout->setContentsMargins(30, 30, 30, 30);
	
	// 标题
	QLabel* titleLabel = new QLabel("管理员控制台");
	titleLabel->setAlignment(Qt::AlignCenter);
	titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #e74c3c; margin-bottom: 20px;");
	layout->addWidget(titleLabel);
	
	// 创建列车管理表格
	adminTrainTable = new QTableWidget();
	adminTrainTable->setColumnCount(3);
	QStringList headers = {"车次", "路线", "状态"};
	adminTrainTable->setHorizontalHeaderLabels(headers);
	
	// 设置表格样式
	adminTrainTable->setStyleSheet(
		"QTableWidget { gridline-color: #bdc3c7; font-size: 12px; }"
		"QTableWidget::item { padding: 8px; }"
		"QHeaderView::section { background-color: #34495e; color: white; padding: 8px; font-weight: bold; }"
	);
	
	adminTrainTable->horizontalHeader()->setStretchLastSection(true);
	adminTrainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	adminTrainTable->setAlternatingRowColors(true);
	
	// 填充列车数据
	auto updateAdminTrainTable = [&]() {
		adminTrainTable->setRowCount(trains.size());
		for (size_t i = 0; i < trains.size(); ++i) {
			const auto& train = trains[i];
			
			// 车次
			adminTrainTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(train.trainNumber)));
			
			// 路线
			QString route = QString::fromStdString(train.stations.front()) + " -> " + QString::fromStdString(train.stations.back());
			adminTrainTable->setItem(i, 1, new QTableWidgetItem(route));
			
			// 状态
			bool isSuspended = find(suspendedTrains.begin(), suspendedTrains.end(), train.trainNumber) != suspendedTrains.end();
			QString status = isSuspended ? "停开" : "正常";
			QTableWidgetItem* statusItem = new QTableWidgetItem(status);
			if (isSuspended) {
				statusItem->setBackground(QBrush(QColor(231, 76, 60, 100))); // 红色背景
			} else {
				statusItem->setBackground(QBrush(QColor(46, 204, 113, 100))); // 绿色背景
			}
			adminTrainTable->setItem(i, 2, statusItem);
		}
	};
	
	updateAdminTrainTable();
	layout->addWidget(adminTrainTable);
	
	// 按钮布局
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	
	QPushButton* suspendBtn = new QPushButton("停开列车");
	suspendBtn->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border: none; padding: 10px 20px; font-size: 14px; border-radius: 5px; }"
							 "QPushButton:hover { background-color: #c0392b; }");
	buttonLayout->addWidget(suspendBtn);
	
	QPushButton* resumeBtn = new QPushButton("复开列车");
	resumeBtn->setStyleSheet("QPushButton { background-color: #27ae60; color: white; border: none; padding: 10px 20px; font-size: 14px; border-radius: 5px; }"
							"QPushButton:hover { background-color: #229954; }");
	buttonLayout->addWidget(resumeBtn);
	
	QPushButton* logoutBtn = new QPushButton("退出登录");
	logoutBtn->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; border: none; padding: 10px 20px; font-size: 14px; border-radius: 5px; }"
							"QPushButton:hover { background-color: #7f8c8d; }");
	buttonLayout->addWidget(logoutBtn);
	
	layout->addLayout(buttonLayout);
	
	// 停开列车按钮事件
	QObject::connect(suspendBtn, &QPushButton::clicked, [mainWindow, updateAdminTrainTable]() {
		int currentRow = adminTrainTable->currentRow();
		if (currentRow < 0) {
			QMessageBox::warning(mainWindow, "提示", "请先选择一个列车!");
			return;
		}
		
		QString trainNumber = adminTrainTable->item(currentRow, 0)->text();
		string trainNumberStr = trainNumber.toStdString();
		
		// 检查是否已经停开
		if (find(suspendedTrains.begin(), suspendedTrains.end(), trainNumberStr) != suspendedTrains.end()) {
			QMessageBox::information(mainWindow, "提示", "该列车已经停开!");
			return;
		}
		
		// 确认停开
		int ret = QMessageBox::question(mainWindow, "确认", 
			QString("确定要停开列车 %1 吗？\n停开后该列车将无法被用户查询和购票。").arg(trainNumber),
			QMessageBox::Yes | QMessageBox::No);
		
		if (ret == QMessageBox::Yes) {
			suspendedTrains.push_back(trainNumberStr);
			saveSuspendedTrainsToDB();
			updateAdminTrainTable();
			QMessageBox::information(mainWindow, "成功", QString("列车 %1 已停开!").arg(trainNumber));
		}
	});
	
	// 复开列车按钮事件
	QObject::connect(resumeBtn, &QPushButton::clicked, [mainWindow, updateAdminTrainTable]() {
		int currentRow = adminTrainTable->currentRow();
		if (currentRow < 0) {
			QMessageBox::warning(mainWindow, "提示", "请先选择一个列车!");
			return;
		}
		
		QString trainNumber = adminTrainTable->item(currentRow, 0)->text();
		string trainNumberStr = trainNumber.toStdString();
		
		// 检查是否已经停开
		auto it = find(suspendedTrains.begin(), suspendedTrains.end(), trainNumberStr);
		if (it == suspendedTrains.end()) {
			QMessageBox::information(mainWindow, "提示", "该列车正在正常运行!");
			return;
		}
		
		// 确认复开
		int ret = QMessageBox::question(mainWindow, "确认", 
			QString("确定要复开列车 %1 吗？\n复开后该列车将恢复正常运行。").arg(trainNumber),
			QMessageBox::Yes | QMessageBox::No);
		
		if (ret == QMessageBox::Yes) {
			suspendedTrains.erase(it);
			saveSuspendedTrainsToDB();
			updateAdminTrainTable();
			QMessageBox::information(mainWindow, "成功", QString("列车 %1 已复开!").arg(trainNumber));
		}
	});
	
	// 退出登录按钮事件
	QObject::connect(logoutBtn, &QPushButton::clicked, [mainWindow]() {
		currentAdmin = nullptr;
		stackedWidget->setCurrentWidget(startMenuWidget);
	});
	
	return widget;
}

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	
	// 初始化数据库
	if (!initDatabase()) {
		QMessageBox::critical(nullptr, "数据库错误", "无法初始化数据库，程序将退出");
		return -1;
	}
	
	// 迁移现有文件数据到数据库
	migrateDataFromFiles();
	
	// 从数据库加载数据
	loadTrainsFromDB();
	loadUsersFromDB();
	loadAdminsFromDB();
	loadSuspendedTrainsFromDB();
	
	// 添加调试信息
	cout << "加载了 " << trains.size() << " 条列车数据" << endl;
	for (const auto& train : trains) {
		cout << "车次: " << train.trainNumber << ", 站点数: " << train.stations.size() << endl;
		cout << "站点: ";
		for (const auto& station : train.stations) {
			cout << station << " ";
		}
		cout << endl;
	}
	
	// 创建主窗口
	QMainWindow mainWindow;
	mainWindow.setWindowTitle("列车购票系统");
	mainWindow.setMinimumSize(1000, 700);
	
	// 创建堆叠窗口部件
	stackedWidget = new QStackedWidget(&mainWindow);
	mainWindow.setCentralWidget(stackedWidget);
	
	// 创建所有界面
	startMenuWidget = createStartMenuWidget(&mainWindow);
	loginWidget = createLoginWidget(&mainWindow);
	registerWidget = createRegisterWidget(&mainWindow);
	adminLoginWidget = createAdminLoginWidget(&mainWindow);
	adminRegisterWidget = createAdminRegisterWidget(&mainWindow);
	mainMenuWidget = createMainMenuWidget(&mainWindow);
	adminMenuWidget = createAdminMenuWidget(&mainWindow);
	
	// 添加到堆叠窗口
	stackedWidget->addWidget(startMenuWidget);
	stackedWidget->addWidget(loginWidget);
	stackedWidget->addWidget(registerWidget);
	stackedWidget->addWidget(adminLoginWidget);
	stackedWidget->addWidget(adminRegisterWidget);
	stackedWidget->addWidget(mainMenuWidget);
	stackedWidget->addWidget(adminMenuWidget);
	
	// 显示开始菜单界面
	stackedWidget->setCurrentWidget(startMenuWidget);
	
	mainWindow.show();
	
	return app.exec();
}
