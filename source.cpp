#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>

using namespace std;

class Date {
	int day;
	int month;
	int year;
public:
	Date() {}
	Date(int day, int month, int year) {
		this->day = day;
		this->month = month;
		this->year = year;
	}
	void showDate() {
		cout << day << "/" << month << "/" << year << endl;
	}

	friend istream& operator>>(istream& input, Date& date) {
		input >> date.day >> date.month >> date.year;
		return input;
	}


	friend ostream& operator<<(ostream& output, Date& date) {
		output << date.day << " " << date.month << " " << date.year << " ";
		return output;
	}
};

class Customer {
public:
	int id;
	string name;
	string surname;
	Date dateOfBirth;
	Customer() {}
	Customer(int id, string name, string surname, Date dateOfBirth) {
		this->id = id;
		this->name = name;
		this->surname = surname;
		this->dateOfBirth = dateOfBirth;
	}
	void showCustomer() {
		cout << "ID: " << id << endl;
		cout << "Name: " << name << endl;
		cout << "Surname: " << surname << endl;
		cout << "BirthDate: ";
		dateOfBirth.showDate();
	}

	void addCustomerInfo() {
		cout << "Enter ID: ";
		cin >> id;
		cout << "Enter Name: ";
		cin >> name;
		cout << "Enter Surname: ";
		cin >> surname;
		cout << "Enter Birth Date: " << endl;
		cin >> dateOfBirth;
	}

};

class Product {
public:
	int id;
	string name;
	double price;
	string color;
	Product()
	{

	}
	Product(int id, string name, double price, string color)
	{
		this->id = id;
		this->name = name;
		this->price = price;
		this->color = color;
	}
	void showProduct() {
		cout << "Id: " << id << endl;
		cout << "name: " << name << endl;
		cout << "price: " << price << endl;
		cout << "color: " << color << endl;
	}
};

class ProductItem {
public:
	int id;
	Product* product;
	int quantity;
	ProductItem()
	{

	}
	ProductItem(int id, Product* product, int quantity)
	{
		this->id = id;
		this->product = product;
		this->quantity = quantity;
	}

	double TotalPrice() { return product->price * quantity; }
};

class Order {
public:
	vector<ProductItem> productItem;
	int custumerId;
	Date datetime;

	Order(vector<ProductItem> productItem, int custumerId)
	{
		this->productItem = productItem;
		this->custumerId = custumerId;
		//indiki tarixi tapmaq ucun 
		time_t now;
		struct tm timeinfo;
		time(&now);
		localtime_s(&timeinfo, &now);
		datetime = Date(timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
	}


	double calculateTotalPrice() {
		double total = 0;
		for (ProductItem& item : productItem) {
			total += item.TotalPrice();
		}
		return total;
	}

};

class Database {
public:

	vector<ProductItem> productItem;
	vector<Order> orders;

	vector<Customer> customers;

	bool Login(Customer customer) {
		for (Customer c : customers)
		{
			if (c.name == customer.name && c.id == customer.id && c.surname == customer.surname)
			{
				return true;
			}
		}
		return false;
	}

	void addCustomer(Customer customer) {
		customers.push_back(customer);

		ofstream rubabFile("customers.txt", ios::app);
		if (rubabFile.is_open()) {
			rubabFile << customer.id << " " << customer.name << " " << customer.surname << " "
				<< customer.dateOfBirth << endl;
			rubabFile.close();
		}
		else {
			cout << "File don't open" << endl;
		}

	}


	vector<Product> products;
	void addProduct(Product product) {
		products.push_back(product);
	}
	void displayProducts() {
		cout << "Product: " << endl;
		for (Product product : products) {
			cout << "ID: " << product.id << " Name: " << product.name << " Price: " << product.price << " Color: " << product.color << endl;
		}
	}

};

class CustomerNotFoundException : exception {
public:
	virtual const char* what() const {
		return "Customer don't found ";
	}
};

class ProductNotFoundException : exception {
public:
	virtual const char* what() const {
		return "Product don't found";
	}
};

void main() {

	Database d;

	Product p1(1, "Cola", 1, "black");
	Product p2(2, "Lays", 3, "Blue");
	Product p3(3, "Magnum", 5, "black");
	Product p4(4, "Haribo", 4, "Colorfulll");
	Product p5(5, "Activia", 3, "Green");

	d.addProduct(p1);
	d.addProduct(p2);
	d.addProduct(p3);
	d.addProduct(p4);
	d.addProduct(p5);

	cout << "Welcome Rubab's shop <3" << endl;

	Customer c1, c2;
	bool isPlay = true;
	while (isPlay) {
		int choose;
		cout << "[1]Login" << endl;
		cout << "[2]Add customer" << endl;
		cout << "[3]Exit" << endl;
		cin >> choose;
		switch (choose) {
		case 1: {
			try {

				Customer c1;
				c1.addCustomerInfo();
				if (d.Login(c1)) {
					d.displayProducts();
					vector<ProductItem> cart;
					char continueAdding = 'y';
					while (continueAdding == 'y') {
						try {
							cout << "Enter product ID to buy: ";
							int productId;
							cin >> productId;
							cout << "Enter quantity: ";
							int quantity;
							cin >> quantity;

							Product* selectedProduct = nullptr;
							for (Product& product : d.products) {
								if (product.id == productId) {
									selectedProduct = &product;
									break;
								}
							}

							if (selectedProduct != nullptr) {
								ProductItem item(cart.size() + 1, selectedProduct, quantity);
								cart.push_back(item);
								cout << "Do you want to continue adding products ? (y/n): ";
								cin >> continueAdding;
							}
							else {
								throw ProductNotFoundException();
							}
						}
						catch (const ProductNotFoundException& e) {
							cout << e.what() << endl;
						}
					}

					Order order(cart, c1.id);

					cout << "Total: " << order.calculateTotalPrice() << endl;
					ofstream rubabFile("receipt.txt", ios_base::app);
					if (rubabFile.is_open()) {
						rubabFile << "----------Order detail:---------- \n";
						rubabFile << "Date: " << order.datetime << "\n";
						rubabFile << "Customer Id: " << c1.id << "\n";
						rubabFile << "-----------------------------------\n";

						for (ProductItem& item : cart) {
							rubabFile << "Product ID: " << item.product->id << " Name: " << item.product->name << " Quantity: " << item.quantity << " Total Price: " << item.TotalPrice() << "\n";
						}
						rubabFile << "--------------------------------------\n";
						rubabFile << "Total Price: " << order.calculateTotalPrice() << "\n";
						rubabFile << "--------------------------------------\n";
						rubabFile << "--------------------------------------\n";

						rubabFile.close();
						cout << "add to receipt.txt " << endl;
					}
					else {
						cout << "File don't found" << endl;
					}
				}
				else {
					throw CustomerNotFoundException();
				}
			}
			catch (CustomerNotFoundException& e) {
				cout << e.what() << endl;
			}
			break;
		}
		case 2:
			c2.addCustomerInfo();
			d.addCustomer(c2);
			break;
		case 3:
			isPlay = false;
			exit(0);
			break;
		default:
			isPlay = false;
			break;
		}
	}
	//for (Customer customer : d.customers) {
	//	customer.showCustomer();
	//	cout << endl;
	//}

}