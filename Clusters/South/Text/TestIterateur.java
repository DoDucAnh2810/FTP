import java.util.Random;

public class TestIterateur {
	public static void main(String[] args) {
		Random r = new Random();
		Sequence<Integer> s2;
		s2 = new SequenceTableau<>();


		for (int i = 0; i < 100; i++) {
			s2.insereQueue(i);
		}
		Iterateur<Integer> it = s2.iterateur();
		System.out.println("Suppression en tete de " + it.prochain());
		it.supprime();
		System.out.println("Suite du parcours :");
		while (it.aProchain()) {
			if (r.nextInt(3) < 1) {
				try {
					System.out.println("Suppression");
					it.supprime();
				} catch (Exception e) {
					System.out.println("Impossible de supprimer (exception " + e);
				}
			} else {
				System.out.println("Avancée : élément " + it.prochain());
			}
		}
		System.out.println("Affichage avec Itérateur : ");
		System.out.print("[ ");
		it = s2.iterateur();
		while (it.aProchain()) {
			System.out.print(it.prochain() + " ");
		}
		System.out.println("]");
	}
}
