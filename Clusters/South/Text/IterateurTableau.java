public class IterateurTableau<Fifi> extends Iterateur<Fifi> {
	int position;
	SequenceTableau<Fifi> s;

	IterateurTableau(SequenceTableau<Fifi> t) {
		s = t;
	}

	@Override
	public boolean aProchain() {
		return position < s.taille;
	}

	@Override
	@SuppressWarnings("unchecked")
	public Fifi prochain() {
		super.prochain();
		int indice = (s.debut+position)%s.elements.length;
		position++;
		return (Fifi) s.elements[indice];
	}

	@Override
	public void supprime() {
		super.supprime();
		assert(position > 0);
		for (int i = position; i<s.taille; i++) {
			int prec = (s.debut+i-1)%s.elements.length;
			int suiv = (prec+1)%s.elements.length;
			s.elements[prec] = s.elements[suiv];
		}
		s.taille--; // Correction après la vidéo
		position--;
	}
}
