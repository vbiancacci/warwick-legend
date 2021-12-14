TGraph* gr;
double GraphToF1(double x){
	return gr->Eval(x,0,"");
}

void XCheck_FluxOverEnergy(){

TTree* tree = new TTree();
tree->ReadFile("FluxOverEnergy.csv","x:y");
Float_t x,y;
tree->SetBranchAddress("x",&x);
tree->SetBranchAddress("y",&y);
const int nEnt = tree->GetEntries();
double X[11] = {0};
double Y[11]= {0};

for(int i = 0; i < nEnt; i++){
tree->GetEntry(i);
X[i] = x;
Y[i] = y;
cout << i << " " << X[i] << " " << Y[i] << endl;
}
TCanvas* c1 = new TCanvas();
c1->SetLogy();
gr = new TGraph(nEnt,X,Y);
gr->SetLineColor(fStyle->Color(1));
gr->Draw("AL");
cout << gr->Integral() << endl;

TF1* f1 = new TF1("f1","GraphToF1(x)",X[0],X[nEnt-1]);
TCanvas* c2 = new TCanvas();
c2->SetLogy();
f1->SetLineColor(fStyle->Color(2));
f1->Draw();

TFile* outFile = new TFile("FluxOverEnergy.root","RECREATE");
gr->Write();
outFile->Close();
}
