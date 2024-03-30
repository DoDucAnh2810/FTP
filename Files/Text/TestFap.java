/*
 * Sokoban - Encore une nouvelle version (à but pédagogique) du célèbre jeu
 * Copyright (C) 2018 Guillaume Huard
 * 
 * Ce programme est libre, vous pouvez le redistribuer et/ou le
 * modifier selon les termes de la Licence Publique Générale GNU publiée par la
 * Free Software Foundation (version 2 ou bien toute autre version ultérieure
 * choisie par vous).
 * 
 * Ce programme est distribué car potentiellement utile, mais SANS
 * AUCUNE GARANTIE, ni explicite ni implicite, y compris les garanties de
 * commercialisation ou d'adaptation dans un but spécifique. Reportez-vous à la
 * Licence Publique Générale GNU pour plus de détails.
 * 
 * Vous devez avoir reçu une copie de la Licence Publique Générale
 * GNU en même temps que ce programme ; si ce n'est pas le cas, écrivez à la Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
 * États-Unis.
 * 
 * Contact:
 *          Guillaume.Huard@imag.fr
 *          Laboratoire LIG
 *          700 avenue centrale
 *          Domaine universitaire
 *          38401 Saint Martin d'Hères
 */

import java.util.Random;

public class TestFap {
	public static void main(String[] args) {
		Random r = new Random();
		FAP<Couple<Double, Integer>> f1, f2;
		f1 = new FAPListe<>();
		f2 = new FAPTableau<>();

		assert (f1.estVide());
		assert (f2.estVide());
		for (int i = 0; i < 100; i++) {
			int code = r.nextInt(2);
			switch(code) {
				case 0:
					Couple<Double, Integer> cp = new Couple<>(r.nextDouble(), r.nextInt(10));
					System.out.println("Insertion de " + cp);
					f1.insere(cp);
					f2.insere(cp);
					break;
				case 1:
					if (f1.estVide() || f2.estVide()) {
						assert (f1.estVide());
						assert (f2.estVide());
						System.out.println("Impossible d'extraire : FAP vide");
					} else {
						Couple<Double, Integer> cp1 = f1.extrait();
						Couple<Double, Integer> cp2 = f2.extrait();
						assert(cp1.equals(cp2));
						System.out.println("Extraction de " + cp1);
					}
					break;
			}
		}
	}
}
