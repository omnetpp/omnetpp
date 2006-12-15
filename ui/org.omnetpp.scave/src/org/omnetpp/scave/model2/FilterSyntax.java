package org.omnetpp.scave.model2;

import java.io.IOException;
import java.io.PushbackReader;
import java.io.Reader;
import java.io.StringReader;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.StringUtils;

// Work in progress...
public class FilterSyntax {
	
	public static Node parseFilter(String filter) {
		Lexer lexer = new Lexer(filter);
		Parser parser = new Parser(lexer);
		return parser.parse();
	}
	
	enum TokenType
	{
		AND,
		OR,
		NOT,
		STRING_LITERAL,
		OP,
		CP,
		END,
	}
	
	public static class Token
	{
		TokenType type;
		String value;
		int startPos, endPos;
		Node parent;
		
		public Token(TokenType type, int startPos, int endPos) {
			this(type, null, startPos, endPos);
		}
		
		public Token(TokenType type, String value, int startPos, int endPos) {
			this.type = type;
			this.value = value;
			this.startPos = startPos;
			this.endPos = endPos;
		}
		
		public TokenType getType() {
			return type;
		}
		
		public String getValue() {
			return value;
		}
		
		public int getStartPos() {
			return startPos;
		}
		
		public int getEndPos() {
			return endPos;
		}
		
		public Node getParent() {
			return parent;
		}
		
		public boolean isEmpty() {
			return startPos == endPos;
		}
		
		public String toString() {
			if (value == null)
				return String.format("%s<%d,%d>", type.name(), startPos, endPos);
			else
				return String.format("%s(%s)<%d,%d>", type.name(), value, startPos, endPos);
		}
	}
	
	public static interface INodeVisitor {
		boolean visit(Node node);
		void visit(Token token);
	}
	
	public static class Node
	{
		public static final int FIELDPATTERN = 0; // fieldName ( pattern )
		public static final int PATTERN = 1;      // pattern
		public static final int NOT = 2;          // NOT <node>
		public static final int AND = 3;          // <node> AND <node>
		public static final int OR = 4;           // <node> OR <node>
		
		int type;
		Object[] content; // Tokens and Nodes
		Node parent;
		
		public Node(Token fieldName, Token open, Token pattern, Token close) {
			type = FIELDPATTERN;
			content = new Object[4];
			content[0] = fieldName;
			content[1] = open;
			content[2] = pattern;
			content[3] = close;
			link();
		}
		
		public Node(Token pattern) {
			type = PATTERN;
			content = new Object[1];
			content[0] = pattern;
			link();
		}
		
		public Node(Token operator, Node operand) {
			type = NOT;
			content = new Object[2];
			content[0] = operator;
			content[1] = operand;
			link();
		}
		
		public Node(Token operator, Node leftOperand, Node rightOperand) {
			type = operator.type == TokenType.AND ? AND : OR;
			content = new Object[3];
			content[0] = leftOperand;
			content[1] = operator;
			content[2] = rightOperand;
			link();
		}
		
		private void link() {
			for (Object obj : content) {
				if (obj instanceof Node)
					((Node)obj).parent = this;
				else if (obj instanceof Token)
					((Token)obj).parent = this;
			}
		}
		
		public Node getParent() {
			return parent;
		}
		
		public int getType() {
			return type;
		}
		
		public Object[] getContent() {
			return content;
		}
		
		public Node getOperand() {
			Assert.isTrue(type == NOT);
			return (Node)content[1];
		}
		
		public Node getLeftOperand() {
			Assert.isTrue(type == OR || type == AND);
			return (Node)content[0];
		}
		
		public Node getRightOperand() {
			Assert.isTrue(type == OR || type == AND);
			return (Node)content[2];
		}
		
		public Token getField() {
			Assert.isTrue(type == FIELDPATTERN);
			return (Token)content[0];
		}
		
		public String getFieldName() {
			return getField().getValue();
		}
		
		public Token getOpeningParen() {
			Assert.isTrue(type == FIELDPATTERN);
			return (Token)content[1];
		}
		
		public Token getClosingParen() {
			Assert.isTrue(type == FIELDPATTERN);
			return (Token)content[3];
		}
		
		public Token getPattern() {
			Assert.isTrue(type == FIELDPATTERN || type == PATTERN);
			if (type == FIELDPATTERN)
				return (Token)content[2];
			else
				return (Token)content[0];
		}
		
		public String getPatternString() {
			return getPattern().getValue();
		}
		
		public void accept(INodeVisitor visitor) {
			boolean cont = visitor.visit(this);
			if (cont) {
				for (Object obj : content) {
					if (obj instanceof Node)
						((Node)obj).accept(visitor);
					else if (obj instanceof Token)
						visitor.visit((Token)obj);
				}
			}
		}
		
		
		public String toString() {
			StringBuffer sb = new StringBuffer();
			format(sb, 0);
			return sb.toString();
		}
		
		private void format(StringBuffer sb, int level) {
			// indent
			sb.append(StringUtils.repeat("  ", level));

			switch (type) {
			case FIELDPATTERN:
				sb.append(getFieldName()).append("(").append(getPatternString()).append(")\n");
				break;
			case PATTERN:
				sb.append(getPatternString()).append("\n");
				break;
			case NOT:
				sb.append("NOT\n");
				if (getOperand() != null)
					getOperand().format(sb, level+1);
				break;
			case AND: case OR:
				sb.append(type == AND ? "AND" : "OR").append("\n");
				if (getLeftOperand() != null)
					getLeftOperand().format(sb, level+1);
				if (getRightOperand() != null)
					getRightOperand().format(sb, level+1);
				break;
			default:
				sb.append("<ERROR>\n");
			}
		}
	}
	
	static class Lexer
	{
		public static final int EOF_CHAR = -1;
		
		PushbackReader input;
		int pos;
		boolean finished;
		
		public Lexer(String input) {
			this(new StringReader(input));
		}
		
		public Lexer(Reader input) {
			this.input = new PushbackReader(input, 1);
			this.pos = 0;
		}
		
		public Token getNextToken() {
			int ch;
			StringBuffer value;
			
			while (true) {
				int startPos = pos;
				ch = getChar();
				switch (ch) {
				case EOF_CHAR: return new Token(TokenType.END, startPos, pos);
				case '(':		return new Token(TokenType.OP, startPos, pos);
				case ')':		return new Token(TokenType.CP, startPos, pos);
				case '"':
					value = new StringBuffer(30);
					while (true) {
						switch (ch = getChar()) {
						case '"': return new Token(TokenType.STRING_LITERAL, value.toString(), startPos, pos);
						case EOF_CHAR: return new Token(TokenType.STRING_LITERAL, value.toString(), startPos, pos);
						default: value.append((char)ch);
						}
					}
				case ' ': case '\n': case '\t': continue;
				default:
					value = new StringBuffer(30);
					value.append((char)ch);
					LOOP:
					while (true) {
						switch (ch = getChar()) {
						case ' ': case '\t': case '\n': case EOF_CHAR: case '(': case ')': ungetChar(ch); break LOOP;
						default: value.append((char)ch); break;
						}
					};
					String v = value.toString();
					if ("AND".equals(v) || "and".equals(v))
						return new Token(TokenType.AND, startPos, pos);
					else if ("OR".equals(v) || "or".equals(v))
						return new Token(TokenType.OR, startPos, pos);
					else if ("NOT".equals(v) || "not".equals(v))
						return new Token(TokenType.NOT, startPos, pos);
						
					return new Token(TokenType.STRING_LITERAL, value.toString(), startPos, pos);
				}
			}
		}
		
		private int getChar() {
			try {
				int ch = input.read();
				if (!finished)
					pos++;
				if (ch == EOF_CHAR)
					finished = true;
				return ch;
			} catch (IOException e) {
				return EOF_CHAR;
			}
		}
		
		private void ungetChar(int ch) {
			try {
				if (ch != EOF_CHAR)
					input.unread(ch);
				pos--;
			} catch (IOException e) {
				// XXX
			}
		}
	}
	
	static class Parser
	{
		Lexer lexer;
		
		Token lookAhead1;
		Token lookAhead2;
		
		public Parser(Lexer lexer) {
			this.lexer = lexer;
		}
		
		/*
		 * expression: 		expr EOF
		 * 
		 * expr:			orExpr
		 * 
		 * orExpr:				andExpr OR orExpr
		 * 					|	andExpr
		 * 
		 * andExpr:				notExpr AND andExpr
		 * 					|	notExpr
		 * 
		 * notExpr:				NOT primaryExpr
		 * 					|	primaryExpr
		 * 
		 * primaryExpr:			( expr )
		 * 					|	LITERAL ( LITERAL )
		 * 					|	LITERAL
		 */
		public Node parse() {
			getNextToken();
			getNextToken();
			return expression();
		}
		
		public Node expression() {
			Node result = expr();
			match(TokenType.END);
			return result;
		}
		
		private Node expr() {
			return orExpr();
		}
		
		private Node orExpr() {
			Node first = andExpr();
			if (lookAhead1.type == TokenType.OR) {
				Token operator = match(TokenType.OR);
				Node second = orExpr();
				return new Node(operator, first, second);
			}
			else
				return first;
		}
		
		private Node andExpr() {
			Node first = notExpr();
			if (lookAhead1.type == TokenType.AND) {
				Token operator = match(TokenType.AND);
				Node second = andExpr();
				return new Node(operator, first, second);
			}
			else
				return first;
		}
		
		private Node notExpr() {
			if (lookAhead1.type == TokenType.NOT) {
				Token operator = match(TokenType.NOT);
				Node expr = primaryExpr();
				return new Node(operator, expr, null);
			}
			else
				return primaryExpr();
		}
 		
		private Node primaryExpr() {
			
			if (lookAhead1.type == TokenType.OP) {
				match(TokenType.OP);
				Node result = expr();
				match(TokenType.CP, true);
				return result;
			}
			else if (lookAhead1.type == TokenType.STRING_LITERAL && lookAhead2.type == TokenType.OP) {
				Token fieldName = match(TokenType.STRING_LITERAL);
				Token open = match(TokenType.OP);
				Token pattern = match(TokenType.STRING_LITERAL, true);
				Token close = match(TokenType.CP, true);
				return new Node(fieldName, open, pattern, close);
			}
			else if (lookAhead1.type == TokenType.STRING_LITERAL) {
				Token pattern = match(TokenType.STRING_LITERAL);
				return new Node(pattern);
			}

			// recover
			while (lookAhead1.type != TokenType.END && lookAhead1.type != TokenType.OP && lookAhead1.type != TokenType.STRING_LITERAL)
				getNextToken();
			
			if (lookAhead1.type != TokenType.END)
				return primaryExpr();
			else {
				Token pattern = match(TokenType.STRING_LITERAL);
				return new Node(pattern);
			}
		}
		
		private Token match(TokenType token) {
			return match(token, false);
		}
		
		private Token match(TokenType tokenExpected, boolean create) {

			if (tokenExpected != lookAhead1.type && !create) {
				while (lookAhead1.type != TokenType.END && lookAhead1.type != tokenExpected) {
					getNextToken();
				}
			}
			
			if (tokenExpected == lookAhead1.type) {
				return getNextToken();
			}
			else { // create
				return new Token(tokenExpected, "", lookAhead1.startPos, lookAhead1.startPos);
			}
		}
		
		private Token getNextToken() {
			Token token = lookAhead1;
			lookAhead1 = lookAhead2;
			lookAhead2 = lexer.getNextToken();
			return token;
		}
	}
}
