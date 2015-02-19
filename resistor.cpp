#include <iostream>
#include <vector>
#include <cassert>
#include <iomanip>

using namespace std;


class Resistor 
{
    float R; // resistance value
    
public:
    Resistor (float x)
        : R(x)
    { 
        // nothing to do
    }
        
    float calc_current (float V) const
    {
        return V / R;
    }
        
    float calc_voltage (float I) const
    {
        return I * R;
    }
        
    float calc_power (float I) const
    {
        return calc_voltage(I) * I;
    }
        
    float get_value() const
    {
        return R;
    }
};

class AbstractCircuit 
{        
public:
	friend bool operator==(const AbstractCircuit &lhs, const AbstractCircuit &rhs) 
	{
		return lhs.calc_resist() == rhs.calc_resist();
	}
	friend ostream& operator<<(ostream &out, const AbstractCircuit &rhs)
	{
		return out << setprecision(4) << rhs.calc_resist() << endl;
	}
    virtual float calc_resist() const = 0;
};

class CircuitLeaf : public AbstractCircuit
{
protected:
    vector<Resistor> R_component;
    
public:
    CircuitLeaf(const vector<Resistor> &v)
        : AbstractCircuit(),
          R_component(v)
    {
       // nothing to do 
    }
    
    void add_resistance(Resistor new_v)
    {
        R_component.push_back(new_v);
    }
};

class Series_RCircuit : public CircuitLeaf
{
public:
    Series_RCircuit(const vector<Resistor> &v)
        : CircuitLeaf(v)
    {
        // nothing to do
    }

    float calc_resist() const 
    {      
        float R = 0;
            
        vector<Resistor>::const_iterator it;
        for (it = R_component.cbegin(); it != R_component.cend(); ++it) {
            R = R + it->get_value();
        }
                
        return R;
    }
};

class Parallel_RCircuit : public CircuitLeaf
{
public:
    Parallel_RCircuit(const vector<Resistor> &v)
        : CircuitLeaf(v)
    {
        // nothing to do
    }
    
    float calc_resist () const 
    {
        float R_inv = 0;
        
        vector<Resistor>::const_iterator it;
        for (it = R_component.begin(); it != R_component.end(); ++it) {
            R_inv = R_inv + 1 / it->get_value();
        }                                                                             
        
        return 1 / R_inv; 
    }
};


class Real_Circuit : public AbstractCircuit
{
    vector<AbstractCircuit*> children;

public:
    Real_Circuit()
        : AbstractCircuit()
    {
        // nothing to do
    }

    void add_child(AbstractCircuit *circuit_part)
    {
        children.push_back(circuit_part);
    }
    
    void remove_child(AbstractCircuit *circuit_part)
    {
        vector<AbstractCircuit*>::iterator it;
        for (it = children.begin(); it != children.end(); ++it) {
            if (*(*it) == (*circuit_part))
                children.erase(it);
        }
    }
    
    float calc_resist () const
    {
        float R_total = 0;
        
        vector<AbstractCircuit*>::const_iterator it;
        for (it = children.cbegin(); it != children.cend(); ++it) {
            R_total += (*it)->calc_resist();
        }
        
        return R_total;
    }
};


int main () 
{
	const double EPS = .01;

    vector<Resistor> v;
    Resistor a(7);
    Resistor b(3);

    v.push_back(a);
    v.push_back(b);

    Series_RCircuit cir_series(v);
    assert(cir_series.calc_resist() - 10 < EPS);

    Parallel_RCircuit cir_parallel(v);
    assert(cir_parallel.calc_resist() - 2.1 < EPS);

    Real_Circuit real;
    real.add_child(&cir_series);
    real.add_child(&cir_parallel);
    assert(real.calc_resist() - 12.1 < EPS);

	cout << cir_parallel;
	cout << real;
	// assert(cir_series2 == real);

	return 0;
}
