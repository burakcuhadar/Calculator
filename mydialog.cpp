#include "mydialog.h"
#include <QtGui>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <cstdlib>


MyDialog::MyDialog(QWidget *parent) : QDialog(parent)
{
    lastPressed = "default";
    isDec = true;
    buttongroup = new QButtonGroup;
    button = new QPushButton*[24];
    glayout = new QGridLayout;
    label = new QLabel;
    label->setStyleSheet("QLabel { background-color : white; color : black; border: 1px solid black }");

    glayout->addWidget(label, 0, 0, 1, -1);

    for(int i=0 ; i < 6 ; i++) {
        for(int j=0 ; j < 4 ; j++) {
            int id = 4*i+j;
            if(id<8) {
                s = QString("");
            } else if(id>=8 && id <18) {
                s = QString::number(id-8);
            } else {
                s = QString('A'+id-18);
            }

            button[id] = new QPushButton(s);
            glayout->addWidget(button[id],i+1,j);
            buttongroup->addButton(button[id], id);
        }
    }
    button[0]->setText(QString('*'));
    button[1]->setText(QString('/'));
    button[2]->setText(QString("Hex"));
    button[3]->setText(QString("Dec"));
    button[4]->setText(QString('+'));
    button[5]->setText(QString('-'));
    button[6]->setText(QString('='));
    button[7]->setText(QString("Clr"));
    connect(buttongroup, SIGNAL(buttonClicked(int)),
            this, SLOT(displayClicked(int)));

    setLayout(glayout);
    setWindowTitle(QString("Project3"));
}

void MyDialog::displayClicked(int id)
{
    string butText = button[id]->text().toUtf8().constData();
    if(string("*/+-").find(butText) != string::npos) {
        if(string("*/+-").find(lastPressed) == string::npos && lastPressed != "default")
            label->setText(label->text() + QString((" " + butText + " ").c_str()));
        else
            return;
    } else if(butText == "Hex") {
        if(isDec) {
            isDec = false;
            label->setText(toHex(label->text()));
        }
        return;
    } else if(butText == "Dec") {
        if(!isDec) {
            isDec = true;
            label->setText(toDec(label->text()));
        }
        return;
    } else if(butText == "Clr") {
        label->setText(QString(""));
        butText = "default";
    } else if(butText == "=") {
    	// if the last pressed button is a number, = button invokes evaluateExpression function
    	if( !isOperator(lastPressed) && lastPressed != "default")
        	evaluateExpression();
        else
            return;
    } else if(string("ABCDEF").find(butText) != string::npos) {
        if(!isDec)
            label->setText(label->text() + QString(butText.c_str()));
        else
            return;
    } else {
        label->setText(label->text() + QString(butText.c_str()));
    }
    lastPressed = butText;
}

QString MyDialog::toDec(QString expr)
{
    int prev=0;
    string text = expr.toUtf8().constData();
    string result = "";
    string token = "";
    for(int i=0; i<=text.size(); i++)
    {
        if( isspace(text[i]) || i == text.size()) {
            token = text.substr(prev, i-prev);

            if(string("+/-*").find(token) == string::npos) {
                long decVal = stol(token, nullptr, 16);
                token = to_string(decVal);
            }
            prev = i+1;
            result += token + " ";
        }
    }
    return QString(result.substr(0,result.length()-1).c_str());
}

QString MyDialog::toHex(QString expr)
{
	int prev=0;
    string text = expr.toUtf8().constData();
    string result = "";
    string token = "";
    for(int i=0; i<=text.size(); i++)
    {
        if( isspace(text[i]) || i == text.size()) {
            token = text.substr(prev, i-prev);
            if(  string("+/-*").find(token) == string::npos  ) {
                long decVal = stol(token,nullptr,10);
                std::stringstream stream;
				stream << std::hex << std::uppercase << abs(decVal);
				std::string hexstr( stream.str() );
				if(decVal < 0 )
                	token = "-" + hexstr;
                else
                	token = hexstr;
            }
            prev = i+1;
            result += token + " ";
        }
    }
	return QString(result.substr(0,result.length()-1).c_str());
}

//if the given string called token is one of + - / = *,  returns true
bool MyDialog::isOperator(string token) { return string("*/+-=").find(token) != string::npos ; }	

//evaluates the expression given in the text of label using Shunting Yard Algorithm
void MyDialog::evaluateExpression()
{
	//store expression
	string expr = label->text().toUtf8().constData();
	//Vector of string to store tokens
	queue<string> tokens;
	//stringstream object to tokenize expr
	stringstream parser(expr);

	string tmp;
	//Start tokenizing
	while( getline(parser, tmp, ' '))
	{
		tokens.push(tmp);
	}

	//value stack to implement Shunting Yard Algorithm
	stack<long> operands;
	//operator stack to implement Shunting Yard Algorithm
	stack<string> operators;

	//start evaluating
	while( !tokens.empty() )
	{
		string currentToken = tokens.front();
		tokens.pop();
		//if the current token is an operator
		if( isOperator( currentToken) )
		{
			// while operators is not empty and the operator at the top has same or greater precedence 
			// over the current operator apply the operator at the top to two elements from top of the operands
			while( !operators.empty() && sameOrGreaterPrecedence( operators.top(), currentToken) )
			{
				applyOperator( operators, operands);
			}
			// finally push the current token
			operators.push( currentToken );
		}
		//if the current token is an operand
		else
		{	
			// if dec mode is on, interpret operands as decimal values
			if(isDec)
				operands.push( stol( currentToken, nullptr, 10) );
			// if hex mode is on, interpret operands as hexadecimal values
			else
				operands.push( stol( currentToken, nullptr, 16) );
		}
	}
	// until there are no operators to apply, apply the operators from the stack
	while( !operators.empty() )
	{
		applyOperator( operators, operands);
	}
	// if dec mode is on, interpret the result as decimal value and set the text of label
	if(isDec)
		label->setText( QString::fromStdString( to_string(operands.top()) ) );
	// if hex mode is on, interpret the result as hexadecimal value and set the text of label
	else
		label->setText( toHex( QString::fromStdString( to_string(operands.top()) ) ));
}

//returns true if operator1 has same or greater precedence than operator2
bool MyDialog::sameOrGreaterPrecedence(string operator1, string operator2)
{
	// if operator1 is * or /, it always have greater or same precedence
	if( operator1 == "*" || operator1 == "/" )
			return true;
	else
		//if operator1 is + or - and operator2 is * or /
		if( operator2 == "/" || operator2 == "*" )
			return false;
		//if operator1 is + or - and operator2 is + or -
		else
			return true;
}

// helper function for evaluateExpression. Applies the operator at the top of operators to the 
// next two operand of operands stack
void MyDialog::applyOperator(stack<string> &operators, stack<long> &operands)
{	
	// stores operator which will be applied
	string topOperator = operators.top();
	operators.pop();
	// stores operands
	long operand1 = operands.top();
	operands.pop();
	long operand2 = operands.top();
	operands.pop();
	long result;
	// according to topOperator the appropriate operation is applied
	switch (topOperator[0]) {
		case '+': result = operand2 + operand1; break;
		case '-': 
		result = operand2 - operand1; break;
		case '*': result = operand2 * operand1; break;
		case '/': result = operand2 / operand1; break;
	}
	// finally, the result is pushed to the stack
	operands.push( result );
}
