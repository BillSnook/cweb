//
//  power.hpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef power_hpp
#define power_hpp


class Power {

	int	pi2c;

public:
	explicit Power();
	
	int getI2CReg( int reg );
	void putI2CReg( int reg, int newValue );
	
	char *getUPS2();

};

#endif /* power_hpp */
