//
//  mtrctl.hpp
//  mtrctl
//
//  Created by William Snook on 4/8/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef mtrctl_hpp
#define mtrctl_hpp

#include "threader.hpp"
#include "sender.hpp"
#include "listen.hpp"

Listener	*listener;
Sender		*sender;
Threader	*threader;

#endif /* mtrctl_hpp */
