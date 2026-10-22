import java.util.ArrayList;
import java.util.Random;

public class Main {
	public static void populate(Integer [][] a, int N) {
		for (int i = 0 ; i < N ; ++i) {
			for (int j = 0 ; j < N ; ++j) {
				a[i][j] = new Random().nextInt(100);
			}
		}
	}

	public static void printMatrix(Integer [][] a,  int N) {
		for (int i = 0 ; i < N ; ++i, System.out.println()) {
			for (int j = 0 ; j < N ; ++j) {
				System.out.print(a[i][j] + " ");
			}
		}
	}

	public static void checker(Integer [][] a, Integer [][] b, int N) {
		for (int i = 0 ; i < N ; ++i) {
			for (int j = 0 ; j < N ; ++j) {
				if (!a[i][j].equals(b[j][i])) {
					System.out.println("Gigele, e groasa!");
					System.exit(-1);
				}
			}
		}

		System.out.println("Binee, tatiiiiiii!");
	}

	public static void main(String [] args) {
		/**
		 * Varianta cu N X N
		 * */
		final int P = Runtime.getRuntime().availableProcessors();
		int N = 10;
		Integer [][] a = new Integer [N][N];
		Integer [][] parMatrix = new Integer[N][N];
		populate(a, N);
		printMatrix(a, N);

		ArrayList<Thread> threads = new ArrayList<>();

		for (int t = 0 ; t < P ; ++t) {
			int id = t;
			threads.add(new Thread(() -> {
				final int start = id * (int) Math.ceil((double) N / P);
				final int end = (int) Math.min((id + 1) * Math.ceil((double) N / P), N);
				for (int i = 0 ; i < N ; ++i) {
					for (int j = start ; j < end ; ++j) {
						parMatrix[i][j] = a[j][i];
					}
				}
			}));
		}

		for (var i : threads) {
			i.start();
		}

		for (var i : threads) {
			try {
				i.join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

		System.out.println("==============");
		printMatrix(parMatrix, N);

		checker(a, parMatrix, N);
	}
}
