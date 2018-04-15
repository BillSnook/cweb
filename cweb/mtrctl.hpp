//
//  mtrctl.hpp
//  mtrctl
//
//  Created by William Snook on 4/8/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef mtrctl_hpp
#define mtrctl_hpp

#include "listen.hpp"
#include "sender.hpp"

int			runLoop = true;
Listener	*listener;
Sender		*sender;

#endif /* mtrctl_hpp */
