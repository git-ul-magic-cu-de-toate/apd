import java.util.ArrayList;
import java.util.Random;

public class MainNM {
	public static void populate(Integer [][] a, int N, int M) {
		for (int i = 0 ; i < N ; ++i) {
			for (int j = 0 ; j < M ; ++j) {
				a[i][j] = new Random().nextInt(100);
			}
		}
	}

	public static void printMatrix(Integer [][] a,  int N, int M) {
		for (int i = 0 ; i < N ; ++i, System.out.println()) {
			for (int j = 0 ; j < M ; ++j) {
				System.out.print(a[i][j] + " ");
			}
		}
	}

	public static void checker(Integer [][] a, Integer [][] b, int N, int M) {
		for (int i = 0 ; i < N ; ++i) {
			for (int j = 0 ; j < M ; ++j) {
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
		 * Am paralelizat pe coloane, idk daca e neaparat mai eficient asa ca din testele mele s-a vazut ca da.
		 * Ca tehnic tu indifernt cum ai face vei avea jump-uri in memorie mai mari fie pe matricea initiala
		 * fie pe aia transpusa.
		 * */
		final int P = Runtime.getRuntime().availableProcessors();
		int N = 10000;
		int M = 5000;
		Integer [][] a = new Integer [N][M];
		Integer [][] parMatrix = new Integer[M][N];
		populate(a, N, M);
		printMatrix(a, N, M);
		ArrayList<Thread> threads = new ArrayList<>();

		for (int t = 0 ; t < P ; ++t) {
			int id = t;
			threads.add(new Thread(() -> {
				final int start = id * (int) Math.ceil((double) M / P);
				final int end = (int) Math.min((id + 1) * Math.ceil((double) M / P), M);
				for (int i = 0 ; i < N ; ++i) {
					for (int j = start ; j < end ; ++j) {
						parMatrix[j][i] = a[i][j];
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
		printMatrix(parMatrix, M, N);
		checker(a, parMatrix, N, M);
	}
}
