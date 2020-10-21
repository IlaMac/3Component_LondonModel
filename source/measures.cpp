//
// Created by ilaria on 2019-11-16.
//

#include "measures.h"

void all_measures(struct Measures &mis, struct H_parameters &Hp, double my_beta, struct Node* Site) {

    unsigned int i, ix, iy, iz, alpha, beta, vec;
    double h_Kinetic = 0., h_Josephson = 0., h_B = 0., h_AB = 0.;
    double F_A = 0;
    double J_0, J_1, J_2;
    double gauge_phase, gauge_phase_0, gauge_phase_1, gauge_phase_2;
    double inv_h2 = 1. / (Hp.h * Hp.h);
    double inv_h = 1. / (Hp.h);
    double h3 = (Hp.h * Hp.h * Hp.h);
    unsigned int nn_ip, ip;
    double phi_shifted_1, phi_shifted_2;
    double qx_min=C_TWO_PI/(Lx);
    double invNorm= 1./((C_TWO_PI)*(C_TWO_PI)*N);
    double Re_rhoz=0.;
    double Im_rhoz=0.;
    double Dx_Ay, Dy_Ax;
    double cos_phi[NC]={0}, sin_phi[NC]={0};
    double inv_N=1./N;
    double J_alpha=0., J_beta=0., DJ_alpha_Dd=0., DJ_beta_Dd=0.;
    double gauge_phase1, gauge_phase2;

    for (ix = 0; ix < Lx; ix++) {
        for (iy = 0; iy < Ly; iy++) {
            for (iz = 0; iz < Lz; iz++) {
                i = ix + Lx * (iy + iz * Ly);
                for (alpha = 0; alpha < 3; alpha++) {
                    /**ENERGY**/
                    //Kinetic= -(1/h²)*\sum_k=1,2,3 |Psi_{alpha}(r)||Psi_{alpha}(r+k)|* cos( theta_{alpha}(r+k) - theta_{alpha}(r) +h*e*A_k(r))
                    for (vec = 0; vec < 3; vec++) {
                        if (vec == 0) {
                            ip = (ix == Lx - 1 ? 0 : ix + 1);
                            nn_ip = ip + Lx * (iy + Ly * iz);
                        } else if (vec == 1) {
                            ip = (iy == Ly - 1 ? 0 : iy + 1);
                            nn_ip = ix + Lx * (ip + Ly * iz);
                        } else if (vec == 2) {
                            ip = (iz == Lz - 1 ? 0 : iz + 1);
                            nn_ip = ix + Lx * (iy + Ly * ip);
                        }
                        gauge_phase = Site[nn_ip].Psi[alpha].t - Site[i].Psi[alpha].t + Hp.h * Hp.e * Site[i].A[vec];
                        //h_Kinetic -= (1. / h2) * O2prod(Site[i].Psi[alpha], NN_Site[i].Psi_plusk[alpha +3*vec]);
                        h_Kinetic -= inv_h2 * cos(gauge_phase);
                    }
                    for (vec = alpha + 1; vec < 3; vec++) {
                        //Josephson= eta* \sum_beta!=alpha |Psi_{alpha}(r)||Psi_{beys}(r)|* cos(theta_{alpha}(r) - theta_{beta}(r))
                        h_Josephson += (Hp.eta * cos(Site[i].Psi[alpha].t - Site[i].Psi[vec].t));
                        if (Hp.e != 0) {
                            //F_{alpha,vec}= A_alpha(r_i) + A_vec(ri+alpha) - A_alpha(r_i+vec) - A_vec(ri)
                            F_A = (Site[i].A[alpha] + Site[nn(i, alpha, 1)].A[vec] - Site[nn(i, vec, 1)].A[alpha] -
                                   Site[i].A[vec]);
                            h_B += ((0.5 * inv_h2) * (F_A * F_A));
                        }
                    }
                }
                //Andreev-Bashkin term = \sum_beta!=alpha \sum_k=1,2,3 -nu*(J^k_alpha *J^k_beta);
                // with J^k_alpha= |Psi_{alpha}(r)||Psi_{alpha}(r+k)|* sin(theta_{alpha}(r+k) - theta_{alpha}(r) +h*e*A_k(r)))
                if (Hp.nu > 0) {
                    for (vec = 0; vec < 3; vec++) {
                        if (vec == 0) {
                            ip = (ix == Lx - 1 ? 0 : ix + 1);
                            nn_ip = ip + Lx * (iy + Ly * iz);
                        } else if (vec == 1) {
                            ip = (iy == Ly - 1 ? 0 : iy + 1);
                            nn_ip = ix + Lx * (ip + Ly * iz);
                        } else if (vec == 2) {
                            ip = (iz == Lz - 1 ? 0 : iz + 1);
                            nn_ip = ix + Lx * (iy + Ly * ip);
                        }
                        gauge_phase_0 = Site[nn_ip].Psi[0].t - Site[i].Psi[0].t + Hp.h * Hp.e * Site[i].A[vec];
                        gauge_phase_1 = Site[nn_ip].Psi[1].t - Site[i].Psi[1].t + Hp.h * Hp.e * Site[i].A[vec];
                        gauge_phase_2 = Site[nn_ip].Psi[2].t - Site[i].Psi[2].t + Hp.h * Hp.e * Site[i].A[vec];
                        J_0 = inv_h * sin(gauge_phase_0);
                        J_1 = inv_h * sin(gauge_phase_1);
                        J_2 = inv_h * sin(gauge_phase_2);
                        h_AB -= inv_h * ((J_0 * J_1) + (J_0 * J_2) + (J_1 * J_2));
                    }
                }
                /**magnetization_chiraliy**/
                phi_shifted_1= Site[i].Psi[1].t - Site[i].Psi[0].t;
                while(phi_shifted_1 >= C_TWO_PI){
                    phi_shifted_1-= C_TWO_PI;}
                while(phi_shifted_1< 0){
                    phi_shifted_1+=C_TWO_PI;}

                phi_shifted_2= Site[i].Psi[2].t - Site[i].Psi[0].t;
                while(phi_shifted_2 >= C_TWO_PI){
                    phi_shifted_2-= C_TWO_PI;}
                while(phi_shifted_2< 0){
                    phi_shifted_2+=C_TWO_PI;}

                if(phi_shifted_1>phi_shifted_2){
                    mis.m+=1;
                }else if(phi_shifted_1<phi_shifted_2){
                    mis.m+=(-1);
                }
                /**dual stiffness**/
                Dx_Ay=(Site[nn(i, 0, 1)].A[1]- Site[i].A[1])/Hp.h;
                Dy_Ax=(Site[nn(i, 1, 1)].A[0]- Site[i].A[0])/Hp.h;

                Re_rhoz+=(cos((double)qx_min*ix)*(Dx_Ay -Dy_Ax));
                Im_rhoz+=(sin((double)qx_min*ix)*(Dx_Ay -Dy_Ax));

                for(alpha=0; alpha<NC; alpha++){
                    /**single phase magnetization**/
                    cos_phi[alpha]+= cos(Site[i].Psi[alpha].t);
                    sin_phi[alpha]+= sin(Site[i].Psi[alpha].t);
                    /**helicity modulus**/

                    ip = (ix == Lx - 1 ? 0 : ix + 1);
                    nn_ip = ip + Lx * (iy + Ly * iz);
                    gauge_phase1=Site[nn_ip].Psi[alpha].t - Site[i].Psi[alpha].t + Hp.h*Hp.e*Site[i].A[vec];
                    J_alpha= inv_h*sin(gauge_phase1);
                    DJ_alpha_Dd= inv_h*cos(gauge_phase1);
                    mis.DH_Ddi[alpha] += (inv_h* J_alpha);
                    mis.D2H_Dd2i[alpha]+= (inv_h*DJ_alpha_Dd ) ;

                    for(beta=0; beta<NC; beta++) {
                        if(beta !=alpha ){
                            gauge_phase2=Site[nn_ip].Psi[beta].t - Site[i].Psi[beta].t + Hp.h*Hp.e*Site[i].A[vec];
                            J_beta= inv_h*sin(gauge_phase2);
                            mis.DH_Ddi[alpha] -= ( Hp.nu *J_beta*DJ_alpha_Dd );
                            mis.D2H_Dd2i[alpha]+=( Hp.nu *J_beta*J_alpha );
                            DJ_beta_Dd=inv_h*cos(gauge_phase2);
                            mis.D2H_Dd2ij[alpha]+= (-Hp.nu*(DJ_alpha_Dd*DJ_beta_Dd));
                        }
                    }
                }

            }
        }
    }

    /**energy**/
    //to compute the heat capacity it is important to consider the total physical energy which is h_tot*h³
    mis.E_kin = (double) h3 * h_Kinetic;
    mis.E_Josephson = (double) h3 * h_Josephson;
    mis.E_B = (double) h3 * h_B;
    mis.E_AB = (double) h3 * h_AB;
    mis.E = (mis.E_kin + mis.E_Josephson + mis.E_B + mis.E_AB);
    /**magnetization_chiraliy**/
    mis.m*=inv_N;
    /**dual stiffness**/
    mis.d_rhoz=invNorm*((Re_rhoz*Re_rhoz) +(Im_rhoz*Im_rhoz));
    /**single phase magnetization**/
    for(alpha=0; alpha<NC; alpha++) {
        cos_phi[alpha]*=inv_N;
        sin_phi[alpha]*=inv_N;
        mis.m_phase[alpha] = (cos_phi[alpha]*cos_phi[alpha]) + (sin_phi[alpha]*sin_phi[alpha]);
    }

}
void energy(struct Measures &mis, struct H_parameters &Hp, double my_beta, struct Node* Site){

    unsigned int i, ix, iy, iz, alpha, vec;
    double h_Kinetic=0., h_Josephson=0., h_B=0., h_AB=0.;
    double F_A=0;
    double J_0, J_1, J_2;
    double gauge_phase, gauge_phase_0,gauge_phase_1, gauge_phase_2;
    double inv_h2=1./(Hp.h*Hp.h);
    double inv_h=1./(Hp.h);
    double h3=(Hp.h*Hp.h*Hp.h);
    unsigned int nn_ip, ip;

    for(ix=0; ix<Lx; ix++){
        for(iy=0; iy<Ly; iy++){
            for(iz=0; iz<Lz; iz++){
                i=ix + Lx * (iy + iz * Ly);
                for(alpha=0; alpha<3; alpha++) {
                    //Kinetic= -(1/h²)*\sum_k=1,2,3 |Psi_{alpha}(r)||Psi_{alpha}(r+k)|* cos( theta_{alpha}(r+k) - theta_{alpha}(r) +h*e*A_k(r))
                    for (vec = 0; vec < 3; vec++) {
                        if(vec==0){
                            ip=(ix == Lx-1 ? 0: ix+1);
                            nn_ip=ip+Lx*(iy+Ly*iz);
                        }else if(vec==1){
                            ip=(iy == Ly-1 ? 0: iy+1);
                            nn_ip=ix+Lx*(ip+Ly*iz);
                        }else if(vec==2){
                            ip=(iz == Lz-1 ? 0: iz+1);
                            nn_ip=ix+Lx*(iy+Ly*ip);
                        }
                        gauge_phase=Site[nn_ip].Psi[alpha].t - Site[i].Psi[alpha].t + Hp.h*Hp.e*Site[i].A[vec];
                        //h_Kinetic -= (1. / h2) * O2prod(Site[i].Psi[alpha], NN_Site[i].Psi_plusk[alpha +3*vec]);
                        h_Kinetic -= inv_h2 *cos(gauge_phase) ;
                    }
                    for (vec = alpha+1; vec < 3; vec++) {
                        //Josephson= eta* \sum_beta!=alpha |Psi_{alpha}(r)||Psi_{beys}(r)|* cos(theta_{alpha}(r) - theta_{beta}(r))
                        h_Josephson += (Hp.eta * cos(Site[i].Psi[alpha].t -  Site[i].Psi[vec].t));
                        if (Hp.e != 0) {
                            //F_{alpha,vec}= A_alpha(r_i) + A_vec(ri+alpha) - A_alpha(r_i+vec) - A_vec(ri)
                            F_A = (Site[i].A[alpha] + Site[nn(i, alpha, 1)].A[vec] - Site[nn(i, vec, 1)].A[alpha] -
                                   Site[i].A[vec]);
                            h_B += ((0.5 *inv_h2) * (F_A * F_A));
                        }
                    }
                }
                //Andreev-Bashkin term = \sum_beta!=alpha \sum_k=1,2,3 -nu*(J^k_alpha *J^k_beta);
                // with J^k_alpha= |Psi_{alpha}(r)||Psi_{alpha}(r+k)|* sin(theta_{alpha}(r+k) - theta_{alpha}(r) +h*e*A_k(r)))
                if(Hp.nu >0 ) {
                    for (vec = 0; vec < 3; vec++) {
                        if(vec==0){
                            ip=(ix == Lx-1 ? 0: ix+1);
                            nn_ip=ip+Lx*(iy+Ly*iz);
                        }else if(vec==1){
                            ip=(iy == Ly-1 ? 0: iy+1);
                            nn_ip=ix+Lx*(ip+Ly*iz);
                        }else if(vec==2){
                            ip=(iz == Lz-1 ? 0: iz+1);
                            nn_ip=ix+Lx*(iy+Ly*ip);
                        }
                        gauge_phase_0=Site[nn_ip].Psi[0].t - Site[i].Psi[0].t + Hp.h*Hp.e*Site[i].A[vec];
                        gauge_phase_1=Site[nn_ip].Psi[1].t - Site[i].Psi[1].t + Hp.h*Hp.e*Site[i].A[vec];
                        gauge_phase_2=Site[nn_ip].Psi[2].t - Site[i].Psi[2].t + Hp.h*Hp.e*Site[i].A[vec];
                        J_0 = inv_h *sin(gauge_phase_0);
                        J_1 = inv_h *sin(gauge_phase_1);
                        J_2 = inv_h *sin(gauge_phase_2);
                        h_AB -= Hp.nu * ( (J_0*J_1) + (J_0*J_2) + (J_1*J_2) );
                    }
                }
            }
        }
    }

    //to compute the heat capacity it is important to consider the total physical energy which is h_tot*h³
    mis.E_kin=(double)h3*h_Kinetic;
    mis.E_Josephson=(double)h3*h_Josephson;
    mis.E_B= (double)h3*h_B;
    mis.E_AB= (double)h3*h_AB;
    mis.E=(mis.E_kin +mis.E_Josephson + mis.E_B + mis.E_AB);
}


void helicity_modulus(struct Measures &mis, struct H_parameters &Hp, struct Node* Site){

    double J_alpha=0., J_beta=0., DJ_alpha_Dd=0., DJ_beta_Dd=0.;
    double gauge_phase1, gauge_phase2;
    unsigned int i, ix, iy, iz, alpha, beta, vec;
    vec=0; //helicity modulus computed along the x direction
    double inv_h=1./(Hp.h);
    unsigned int nn_ip, ip;

    ip=(ix == Lx-1 ? 0: ix+1);
    nn_ip=ip+Lx*(iy+Ly*iz);

    for(ix=0; ix<Lx;ix++){
        for(iy=0; iy<Ly;iy++){
            for(iz=0; iz<Lz;iz++){
                i=ix +Lx*(iy+Ly*iz);
                for(alpha=0; alpha<NC; alpha++){
                    gauge_phase1=Site[nn_ip].Psi[alpha].t - Site[i].Psi[alpha].t + Hp.h*Hp.e*Site[i].A[vec];

                    J_alpha= inv_h*sin(gauge_phase1);
                    DJ_alpha_Dd= inv_h*cos(gauge_phase1);

                    mis.DH_Ddi[alpha] += (inv_h* J_alpha);
                    mis.D2H_Dd2i[alpha]+= (inv_h*DJ_alpha_Dd ) ;

                    for(beta=0; beta<NC; beta++) {
                        if(beta !=alpha ){
                            gauge_phase2=Site[nn_ip].Psi[beta].t - Site[i].Psi[beta].t + Hp.h*Hp.e*Site[i].A[vec];
                            J_beta= inv_h*sin(gauge_phase2);
                            mis.DH_Ddi[alpha] -= ( Hp.nu *J_beta*DJ_alpha_Dd );
                            mis.D2H_Dd2i[alpha]+=( Hp.nu *J_beta*J_alpha );
                            DJ_beta_Dd=inv_h*cos(gauge_phase2);
                            mis.D2H_Dd2ij[alpha]+= (-Hp.nu*(DJ_alpha_Dd*DJ_beta_Dd));
                        }
                    }
                }
            }
        }
    }

}


void dual_stiffness(struct Measures &mis, struct H_parameters &Hp, struct Node* Site){

    double qx_min=C_TWO_PI/(Lx);
    double invNorm= 1./((C_TWO_PI)*(C_TWO_PI)*N);
    unsigned int i, ix, iy, iz;
    double Re_rhoz=0.;
    double Im_rhoz=0.;
    double Dx_Ay, Dy_Ax;

    for(ix=0; ix<Lx;ix++){
        for(iy=0; iy<Ly;iy++){
            for(iz=0; iz<Lz;iz++){
                i=ix +Lx*(iy+Ly*iz);
                Dx_Ay=(Site[nn(i, 0, 1)].A[1]- Site[i].A[1])/Hp.h;
                Dy_Ax=(Site[nn(i, 1, 1)].A[0]- Site[i].A[0])/Hp.h;

                Re_rhoz+=(cos((double)qx_min*ix)*(Dx_Ay -Dy_Ax));
                Im_rhoz+=(sin((double)qx_min*ix)*(Dx_Ay -Dy_Ax));
            }
        }
    }
    mis.d_rhoz=invNorm*((Re_rhoz*Re_rhoz) +(Im_rhoz*Im_rhoz));
}

void magnetization(struct Measures &mis, struct Node* Site){
    //The Ising parameter m(x,y)=+/-1 indicates the chirality of the three phases. If the phases are ordered as: phi_1, phi_2, phi_3 then m=1; otherwise if the order is phi_1, phi_3, phi_2 then m=-1.
    unsigned ix, iy, iz, i;

    double phi_shifted_1=0.;
    double phi_shifted_2=0.;

    for(iz=0; iz<Lz;iz++) {
        for (iy = 0; iy < Ly; iy++) {
            for (ix = 0; ix < Lx; ix++) {
                i=ix +Lx*(iy+Ly*iz);

                phi_shifted_1= Site[i].Psi[1].t - Site[i].Psi[0].t;
                while(phi_shifted_1 >= C_TWO_PI){
                        phi_shifted_1-= C_TWO_PI;}
                while(phi_shifted_1< 0){
                        phi_shifted_1+=C_TWO_PI;}

                phi_shifted_2= Site[i].Psi[2].t - Site[i].Psi[0].t;
                while(phi_shifted_2 >= C_TWO_PI){
                        phi_shifted_2-= C_TWO_PI;}
                while(phi_shifted_2< 0){
                        phi_shifted_2+=C_TWO_PI;}

                if(phi_shifted_1>phi_shifted_2){
                    mis.m+=1;
                        }else if(phi_shifted_1<phi_shifted_2){
                    mis.m+=(-1);
                }
            }
        }
    }
    mis.m=mis.m/N;
}

void magnetization_singlephase(struct Measures &mis, struct Node* Site){
    //The Ising parameter m(x,y)=+/-1 indicates the chirality of the three phases. If the phases are ordered as: phi_1, phi_2, phi_3 then m=1; otherwise if the order is phi_1, phi_3, phi_2 then m=-1.
    unsigned ix, iy, iz, i, alpha;
    double cos_phi[NC]={0}, sin_phi[NC]={0};
    double inv_N=1./N;
    for(iz=0; iz<Lz;iz++) {
        for (iy = 0; iy < Ly; iy++) {
            for (ix = 0; ix < Lx; ix++) {
                i=ix +Lx*(iy+Ly*iz);
                for(alpha=0; alpha<NC; alpha++){
                    cos_phi[alpha]+= cos(Site[i].Psi[alpha].t);
                    sin_phi[alpha]+= sin(Site[i].Psi[alpha].t);
                }
            }
        }
    }

    for(alpha=0; alpha<NC; alpha++) {
        cos_phi[alpha]*=inv_N;
        sin_phi[alpha]*=inv_N;
        mis.m_phase[alpha] = (cos_phi[alpha]*cos_phi[alpha]) + (sin_phi[alpha]*sin_phi[alpha]);
    }
}

void save_lattice(struct Node* Site, const fs::path & directory_write, std::string configuration){

    std::string sPsi;
    std::string sA;
    sPsi= std::string("Psi_")+ configuration + std::string(".bin");
    sA= std::string("A_")+ configuration + std::string(".bin");
    fs::path psi_init_file = directory_write / sPsi;
    fs::path a_init_file = directory_write / sA;

    FILE *fPsi= nullptr;
    FILE *fA= nullptr;
    unsigned int i=0;

    if((fPsi=fopen(psi_init_file.c_str(), "w"))) {
        for (i = 0; i < N; i++) {
            fwrite(Site[i].Psi, sizeof(struct O2), 3, fPsi);
        }
        fclose(fPsi);
    }

    if((fA=fopen(a_init_file.c_str(), "w"))) {
    	for (i = 0; i < N; i++) {
            fwrite(Site[i].A, sizeof(double), 3, fA);
	}
        fclose(fA);
    }

}

