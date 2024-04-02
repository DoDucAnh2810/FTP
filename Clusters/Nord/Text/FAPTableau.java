public class FAPTableau<Bill extends Comparable<Bill>> extends FAP<Bill> {
	SequenceTableau<Bill> s;

	FAPTableau() {
		s = new SequenceTableau<>();
		super.s = s;
	}

	@Override
	@SuppressWarnings("unchecked")
	void insere(Bill element) {
		s.insereTete(element);
		int position = 0;
		int courant = (s.debut+position)%s.elements.length;
		int suivant = (courant+1)%s.elements.length;
		Bill eltCourant = (Bill) s.elements[courant];
		Bill eltSuivant = (Bill) s.elements[suivant];
		while ((position < s.taille-1) && (eltCourant.compareTo(eltSuivant) > 0)) {
			s.elements[courant] = eltSuivant;
			s.elements[suivant] = eltCourant;
			position++;
			courant = suivant;
			suivant = (courant+1)%s.elements.length;
			eltSuivant = (Bill) s.elements[suivant];
		}
	}
}
