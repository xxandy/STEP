# Author  : Ayako Iwasaki
# Project : STEP2020
# Reference: https://github.com/xharaken/step2/blob/master/modularized_calculator.py

def readNumber(line, index):
    """
    - Take a line as input and tokenize numbers until it encounters non-number.
    - Return a token whose type is number and advanced index.
    """
    number = 0
    while index < len(line) and line[index].isdigit():
        number = number * 10 + int(line[index])
        index += 1
    if index < len(line) and line[index] == '.':
        index += 1
        keta = 0.1
        while index < len(line) and line[index].isdigit():
            number += int(line[index]) * keta
            keta /= 10
            index += 1
    token = {'type': 'NUMBER', 'number': number}
    return token, index


def readSymbol(symbol, line, index):
    """
    - Take a line as input and process a symble.
    - Return a token whose type is symbol and advanced index
    """
    dic = {'+': 'PLUS', '-': 'MINUS', '*': 'MULTIPLY', '/': 'DIVIDE',
           '(': 'OPENING_BRACKET', ')': 'CLOSING_BRACKET'}
    if dic.get(symbol):
        token = {'type': dic[symbol]}
    else:
        print('Invalid character found: ' + line[index])
        exit(1)
    return token, index + 1


def tokenize(line):
    """
    - Take a line as an input and return tokens.
    """
    tokens = []
    index = 0
    while index < len(line):
        if line[index].isdigit():
            (token, index) = readNumber(line, index)
        else:
            (token, index) = readSymbol(line[index], line, index)
        tokens.append(token)
    return tokens


def processBrackets(tokens):
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
                print('Invalid Syntax: No matching brackets')
                exit(1)
            # Extract tokens between opening and closing bracket
            tokensInBracket = tokens[lastOpeningBracketIndex + 1:index]
            # Insert a dummy '+'
            tokensInBracket.insert(0, {'type': 'PLUS'})
            # Process mulplication and division in the bracket first
            tokensWithoutMultiplyDivide = processMutiplyDivide(tokensInBracket)
            # Then, process plus and minus to get the calculation result inside the bracket
            bracketCalcResult = processPlusMinus(tokensWithoutMultiplyDivide)
            # Delete tokens in brackets and insert the calculation result
            deleteIndex = range(lastOpeningBracketIndex, index + 1)
            tokens = [tokens[i] for i in range(len(tokens)) if i not in deleteIndex]
            tokens.insert(
                lastOpeningBracketIndex, {
                    'type': 'NUMBER', 'number': bracketCalcResult})
            # Call this function recursively until there is no brackets in tokens
            return processBrackets(tokens)
        index += 1
    # new_token = tokens
    return tokens


def processMutiplyDivide(tokens):
    """
    - Take tokens without brackets as an input process multiplication and division.
    - Return tokens without multiplication and division.
    """
    index = 1
    deleteIndex = []
    while index < len(tokens):
        if tokens[index]['type'] == 'NUMBER':
            if tokens[index - 1]['type'] == 'MULTIPLY':
                # Example: [..., NUMBER (2), MULTIPLY, NUMBER (3), ...] where index is at NUMBER (3)
                # Update NUMBER (2) with the calculation result (6)
                tokens[index - 2]['number'] *= tokens[index]['number']
                # Store indexes of [MULTIPLY, NUMBER (3)] to delete later
                deleteIndex.extend([index - 1, index])
            if tokens[index - 1]['type'] == 'DIVIDE':
                # If division by zero is detected, exit
                if (tokens[index]['number'] == 0):
                    print('division by zero')
                    exit(1)
                # Example: [..., NUMBER (2), DIVIDE, NUMBER (3), ...] where index is at NUMBER (3)
                # Update NUMBER (2) with the calculation result (1.5)
                tokens[index - 2]['number'] /= tokens[index]['number']
                # Store indexes of [DIVIDE, NUMBER (3)] to delete later
                deleteIndex.extend([index - 1, index])
        index += 1
    # Delete tokens whose index is in deleteIndex
    tokens = [tokens[i] for i in range(len(tokens)) if i not in deleteIndex]
    return tokens


def processPlusMinus(tokens):
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
                print('Invalid syntax')
                exit(1)
        index += 1
    return answer


def evaluateLine(line):
    """
    - Take a line as an input and return the calculation result as a number.
    """
    if line == '':
        print('Invalid input: please type something')
        exit(0)
    # Tokenize a line
    tokens = tokenize(line)
    tokens.insert(0, {'type': 'PLUS'})  # Insert a dummy '+' token
    # First, process brackets.
    tokensWithoutBrackets = processBrackets(tokens)
    # Then, process multiplication and division.
    tokensWithoutMultiplyDivide = processMutiplyDivide(tokensWithoutBrackets)
    # Finally, process addition and substraction to get the answer.
    answer = processPlusMinus(tokensWithoutMultiplyDivide)
    return answer


def test(line):
    actualAnswer = evaluateLine(line)
    expectedAnswer = eval(line)
    if abs(actualAnswer - expectedAnswer) < 1e-8:
        print("PASS! (%s = %f)" % (line, expectedAnswer))
    else:
        print("FAIL! (%s should be %f but was %f)" %
              (line, expectedAnswer, actualAnswer))


# Add more tests to this function :)
def runTest():
    print("==== Test started! ====")
    test("1+2")
    test("1.0+2.1-3")
    test("1+2*3.5")
    test("1+3/5+7.3")
    test("5*1")
    test("5+(2+3/(4-7))")
    test("2++3")
    print("==== Test finished! ====\n")


runTest()

while True:
    print('> ', end="")
    line = input()
    answer = evaluateLine(line)
    print("answer = %f\n" % answer)
