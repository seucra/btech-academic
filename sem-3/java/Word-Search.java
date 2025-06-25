/* 
* Took around 3hrs
*/

import java.util.Scanner;

class Solution {
	private int[] find0(char[][] board, char alpha, int skip) {
// returns position of unused starting index-> aplha || returns empty array to indicate failiure
		int[] ret = {-1,-1};
		int count = 0;
		for (int i = 0; i < board.length; i++) {
			for (int j = 0; j < board[0].length; j++) {
				if (board[i][j] == alpha) {
					if (count == skip - 1)
						return new int[]{i, j};
					count++;
				}
			}
		}
		return new int[]{-1, -1};

	}

	private int[][] adjpos(char[][] board, char alpha, int[] rootpos) {
// returns array of adjacent positions haiving alpha or return an empty array known my res[0] == {0,0}
		int[][] res = new int[4][2];
		int k=0;
		int r = board.length;
		int c = board[0].length;
		int[] dx = {-1,1,0,0};
		int[] dy = {0,0,-1,1};
		for (int i=0; i<4; i++) {
			int nr = r + dx[i];
			int nc = c + dy[i];
			if (nr>=0 && nc>=0 && nr<r && nc<c && (board[nr][nc] == alpha))
				res[k++] = new int[]{nr,nc};
		}
		return res;
	}

	private boolean checkinsideroot(char[][] board, String word, int index, int[] rootpos) {
// want this to work like climbing a tree
		if (index == word.length())	return true;
		int[][] res = adjpos(board, word.charAt(index), rootpos);
		if (res[0] == new int[]{0,0})	return false;
		for (int i=0; i<4; i++) {
			boolean next = checkinsideroot(board, word, index+1, res[i]);
			if (next == true)	return true;
		}
		return false;
	}

	private boolean startcheck(char[][] board, String word) {
		int attempt = 0; //no of attempts
		while (true) {
			int[] root = find0(board, word.charAt(0), ++attempt); //index of first
			if (root == new int[]{0,0}) {
				System.out.println("~attempts required:"+attempt+".~\n\tNo such String found.");
				return false;
			}
			int index = 1;
			boolean found = checkinsideroot (board, word, index, root);
			System.out.println("~attempts required:"+attempt+".~");
			return found;
		}
	}

	public boolean exist(char[][] board, String word) {
		boolean found = startcheck(board, word);
		if (found) {
			System.out.println("\t\tString Found");
			return true;
		}
		return false;
	}
}

class WordSearch {
	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		System.out.println("Enter row and column:  ");
		int r = sc.nextInt();
		int c = sc.nextInt();

		char[][] board = new char[r][c];
		System.out.println("\nEnter data (1 character at a time):");
		for (int i = 0; i < r; i++) {
			System.out.println("Row " + (i + 1) + ":");
			for (int j = 0; j < c; j++) {
				System.out.print("  Column " + (j + 1) + ": ");
				board[i][j] = sc.next().charAt(0);  // read one character
			}
		}

		System.out.println("\nBoard:");
		for (int i=0; i<r; i++) {
			for (int j = 0; j < c && j < board[0].length; j++)
				System.out.print("  "+board[i][j]);
			System.out.println();
		}
		sc.nextLine();
		sc.nextLine();
		System.out.println("Enter String to search for: ");
		String word = sc.nextLine();

		Solution solver = new Solution();
		boolean result = solver.exist(board, word);
		System.out.println(result ? "Word found." : "Word not found.");
	}
}
