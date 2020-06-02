# Author  : Ayako Iwasaki
# Project : STEP2020
# Reference: https://github.com/xharaken/step2/blob/master/modularized_calculator.py

class tokenizer:
    def __readNumber(self):
        """
        - Move index forward and tokenize numbers until it encounters non-number.
        - Return a token whose type is number.
        """
        number = 0
        while self.__index < len(self.__line) and self.__line[self.__index].isdigit():
            number = number * 10 + int(self.__line[self.__index])
            self.__index += 1
        if self.__index < len(self.__line) and self.__line[self.__index] == '.':
            self.__index += 1
            keta = 0.1
            while self.__index < len(self.__line) and self.__line[self.__index].isdigit():
                number += int(self.__line[self.__index]) * keta
                keta /= 10
                self.__index += 1
        token = {'type': 'NUMBER', 'number': number}
        return token

    def __readSymbol(self, symbol):
        """
        - Process a symble and move index forward.
        - Return a token whose type is symbol.
        """
        dic = {'+': 'PLUS', '-': 'MINUS', '*': 'MULTIPLY', '/': 'DIVIDE',
               '(': 'OPENING_BRACKET', ')': 'CLOSING_BRACKET'}
        if dic.get(symbol):
            token = {'type': dic[symbol]}
        else:
            raise Exception('Error: invalid character "' + self.__line[self.__index] + '"')
        self.__index += 1
        return token

    def tokenize(self, line):
        """
        - Take a line as an input and return tokens.
        """
        self.__index = 0
        self.__tokens = []
        self.__line = line
        while self.__index < len(line):
            if line[self.__index].isdigit():
                token = self.__readNumber()
            else:
                token = self.__readSymbol(line[self.__index])
            self.__tokens.append(token)
        return self.__tokens


class calculator:
    def __processPlusMinus(self, tokens):
        """
        - Take tokens without symbols other than PLUS and MINUS as an input.
        - Return the calculation result as a number.
        """
        answer = 0
        index = 1
        while index < len(tokens):
            if tokens[index]['type'] == 'NUMBER':
                if tokens[index - 1]['type'] == 'PLUS':
                    answer += tokens[index]['number']
                elif tokens[index - 1]['type'] == 'MINUS':
                    answer -= tokens[index]['number']
                else:
                    raise Exception('Error: invalid syntax')
            index += 1
        return answer

    def __processMutiplyDivide(self, tokens):
        """
        - Take tokens without brackets as an input process multiplication and division.
        - Return tokens without multiplication and division.
        """
        index = 1
        deleteIndex = []
        while index < len(tokens):
            if tokens[index]['type'] == 'NUMBER':
                if tokens[index - 1]['type'] == 'MULTIPLY':
                    # ex) [..., NUMBER (2), MULTIPLY, NUMBER (3), ...] where index is at NUMBER (3)
                    # Update NUMBER (2) with the calculation result (6)
                    tokens[index - 2]['number'] *= tokens[index]['number']
                    # Store indexes of [MULTIPLY, NUMBER (3)] to delete later
                    deleteIndex.extend([index - 1, index])
                if tokens[index - 1]['type'] == 'DIVIDE':
                    # If division by zero is detected, raise error
                    if (tokens[index]['number'] == 0):
                        raise Exception("Error: division by zero")
                    # ex) [..., NUMBER (2), DIVIDE, NUMBER (3), ...] where index is at NUMBER (3)
                    # Update NUMBER (2) with the calculation result (1.5)
                    tokens[index - 2]['number'] /= tokens[index]['number']
                    # Store indexes of [DIVIDE, NUMBER (3)] to delete later
                    deleteIndex.extend([index - 1, index])
            index += 1
        # Delete tokens whose index is in deleteIndex
        tokens = [tokens[i] for i in range(len(tokens)) if i not in deleteIndex]
        return tokens

    def __processFourArithmeticOperations(self, tokens):
        """
        - Take tokens without brackets as an input.
        - Return the calculation result as a number.
        """
        # Process Multiplication and division first
        tokensWithoutMultiplyDivide = self.__processMutiplyDivide(tokens)
        # Then, process addition and substraction to get the answer.
        answer = self.__processPlusMinus(tokensWithoutMultiplyDivide)
        return answer

    def __processBrackets(self, tokens):
        """
        - Take tokens as an input and calculate inside brackets.
        - Return tokens without brackets.
        - This function is called recursively until there is no brackets in the tokens.
        """
        index = 1
        lastOpeningBracketIndex = None
        while index < len(tokens):
            # Store the index of the last opening bracket
            if tokens[index]['type'] == 'OPENING_BRACKET':
                lastOpeningBracketIndex = index
            # Process brackets from inntermost one by looking for a first closing bracket
            if tokens[index]['type'] == 'CLOSING_BRACKET':
                # If there is no opening bracket before a closing bracket, it is invalid
                if not lastOpeningBracketIndex:
                    raise Exception('Error: no matching brackets')
                # Extract tokens between opening and closing bracket
                tokensInBracket = tokens[lastOpeningBracketIndex + 1:index]
                # Insert a dummy '+' calculate inside the brackets
                tokensInBracket.insert(0, {'type': 'PLUS'})
                bracketCalcResult = self.__processFourArithmeticOperations(tokensInBracket)
                # Delete tokens in brackets and insert the calculation result
                deleteIndex = range(lastOpeningBracketIndex, index + 1)
                tokens = [tokens[i] for i in range(len(tokens)) if i not in deleteIndex]
                tokens.insert(
                    lastOpeningBracketIndex, {
                        'type': 'NUMBER', 'number': bracketCalcResult})
                # Call this function recursively until there is no brackets in tokens
                return self.__processBrackets(tokens)
            index += 1
        return tokens

    def evaluate(self, line):
        """
        - Take a line as an input and return the calculation result as a number.
        """
        if line == '':
            raise Exception('Error: please type something')
        # Tokenize a line
        tokenizerObj = tokenizer()
        tokens = tokenizerObj.tokenize(line)
        # Insert a dummy '+' token
        tokens.insert(0, {'type': 'PLUS'})
        # Process brackets and then calculate to get the answer
        tokensWithoutBrackets = self.__processBrackets(tokens)
        answer = self.__processFourArithmeticOperations(tokensWithoutBrackets)
        return answer


def test(line, expectedError=None):
    calculatorObj = calculator()
    # If no error is expected
    if not expectedError:
        expectedAnswer = eval(line)
        calculatedAnswer = calculatorObj.evaluate(line)
        if abs(calculatedAnswer - expectedAnswer) < 1e-8:
            print("PASS! '%s' = %f" % (line, expectedAnswer))
        else:
            print("FAIL! '%s' should be %f but was %f" %
                  (line, expectedAnswer, calculatedAnswer))
    else:  # If an error is expected
        try:
            calculatedAnswer = calculatorObj.evaluate(line)
        except Exception as e:
            errorMsg = str(e)
            if expectedError in errorMsg:
                print("PASS! '%s' raises '%s'" % (line, errorMsg))
            else:
                print("FAIL! '%s' should raise '%s' but raised '%s'" %
                      (line, expectedError, errorMsg))
        else:  # If succeeded although an error is expected
            print("FAIL! '%s' did not raise '%s'" %
                  (line, expectedError))


def runTest():
    print("==== Test started! ====")
    test("1+2")
    test("1-2")
    test("1.0+2.1-3")
    test("1+2*3.5")
    test("1+3/5+7.3")
    test("5*1")
    test("5/1")
    test("2*(1+3)")
    test("5+(2+3/(4-7))")
    test("2*(1/(4.4*5)+3)+5.6*(2+3/(4.6-7))")
    test("1.999999999999999999999*4")
    test("", expectedError='please type something')
    test("1+3)", expectedError='no matching brackets')
    test("4/0", expectedError='division by zero')
    test("5%1", expectedError='invalid character')
    print("==== Test finished! ====\n")


if __name__ == '__main__':
    runTest()
    calculatorObj = calculator()
    while True:
        print('> ', end="")
        line = input()
        try:
            answer = calculatorObj.evaluate(line)
            print("Answer: %f\n" % answer)
        except Exception as e:
            print(e)
